
#include "srslte/srslte.h"
#include <memory.h>
#include "srslte/utils/debug.h"


void SetConfiguration(
	srslte_cell_t* cell,
	srslte_prach_cfg_t* prach_cfg,
	srslte_refsignal_dmrs_pusch_cfg_t* puschConfig,
	srslte_pusch_hopping_cfg_t* hoppingConfig,
	srslte_pucch_cfg_t* pucchConfig)
{
	cell->id = 0;
	cell->cp = SRSLTE_CP_NORM;
	cell->nof_ports = 1;
	cell->nof_prb = 50;
	cell->phich_length = SRSLTE_PHICH_NORM;
	cell->phich_resources = SRSLTE_PHICH_R_1;

	// Generate SIB1
	// PRACH Configuration
	prach_cfg->root_seq_idx = 41;
	prach_cfg->hs_flag = false;
	prach_cfg->config_idx = 4;
	prach_cfg->freq_offset = 2;
	prach_cfg->zero_corr_zone = 11;


	// PUSCH configuration
	puschConfig->cyclic_shift = 0;
	puschConfig->delta_ss = 0;
	puschConfig->group_hopping_en = false;
	puschConfig->sequence_hopping_en = false;

	// PUCCH configuration
	pucchConfig->delta_pucch_shift = 1;
	pucchConfig->n_rb_2 = 2;
	pucchConfig->N_cs = 0;
	pucchConfig->n1_pucch_an = 12;
	pucchConfig->srs_configured = false;

	// Hopping configuration
	hoppingConfig->hop_mode = 1;
	hoppingConfig->n_sb = 1;
	hoppingConfig->hopping_offset = 4;
}

int DecodeDataChannel(srslte_enb_ul_t* uplinkConfig)
{
	srslte_ra_ul_dci_t      grant;
	srslte_dci_location_t   location;
	srslte_softbuffer_rx_t softbuffer;

	// Copy grant info
	memset(&grant, 0, sizeof(srslte_ra_dl_dci_t));
	memset(&location, 0, sizeof(srslte_dci_location_t));
	grant.mcs_idx = 6;
	srslte_softbuffer_rx_init(&softbuffer, uplinkConfig->cell.nof_prb);

	int res = -1;

	srslte_ra_ul_dci_t dci; // Downlink Control Information
	dci.freq_hop_fl = -1;
	dci.type2_alloc.L_crb = 50;
	dci.type2_alloc.RB_start = 0;
	dci.mcs_idx = 6;

	srslte_ra_ul_grant_t ulGrant;

	if (srslte_ra_ul_dci_to_grant(&dci, uplinkConfig->cell.nof_prb, 0, &ulGrant, 0) != SRSLTE_SUCCESS)
	{
		printf("Error calculating the pusch grant\n");
		return SRSLTE_ERROR;
	}

	srslte_uci_data_t uci_data;
	memset(&uci_data, 0, sizeof(srslte_uci_data_t));
	uint8_t *data = malloc((1024 / 8) * 150);

	srslte_enb_ul_get_pusch(uplinkConfig,
		&ulGrant,
		&softbuffer,
		0,           // rnti_idx
		grant.rv_idx,
		0,           // current_tx_nb,
		data,        // data
		&uci_data,
		0            // tti
	);

	fprintf(stderr, "DecodeDataChannel returning: %d\n", res);

	return res;
}

void CalculateEVM(cf_t* signal, const unsigned int signalLen, unsigned int* resultSize, cf_t** distances, cf_t** CEs, short** LLRs)
{
	srslte_cell_t cell;
	srslte_enb_ul_t uplinkConfig;
	srslte_prach_cfg_t prach_cfg;
	srslte_refsignal_dmrs_pusch_cfg_t puschConfig;
	srslte_pusch_hopping_cfg_t hoppingConfig;
	srslte_pucch_cfg_t pucchConfig;

	SetConfiguration(&cell, &prach_cfg, &puschConfig, &hoppingConfig, &pucchConfig);

	if (srslte_enb_ul_init(&uplinkConfig, cell, &prach_cfg, &puschConfig, &hoppingConfig, &pucchConfig, 1))
	{
		printf("Failed to init uplink\n");
		return;
	}

	*resultSize = uplinkConfig.pusch.max_re;
	*LLRs = srslte_vec_malloc(sizeof(int16_t) * *resultSize * srslte_mod_bits_x_symbol(SRSLTE_MOD_QPSK));
	if (!LLRs) {
		printf("Failed to allocate q\n");
		return;
	}

	*distances = srslte_vec_malloc(sizeof(cf_t) * *resultSize);
	if (!distances) {
		printf("Failed to allocate d\n");
		return;
	}

	*CEs = srslte_vec_malloc(sizeof(cf_t) * *resultSize);
	if (!CEs) {
		printf("Failed to allocate d\n");
		return;
	}

	uplinkConfig.pusch.q = *LLRs;
	uplinkConfig.pusch.d = *distances;
	uplinkConfig.pusch.ce = *CEs;

	srslte_pucch_set_threshold(&uplinkConfig.pucch, 1, 0);

	srslte_enb_ul_cfg_rnti(&uplinkConfig, 0, 1);
	
	// Perform fft on the signal
	srslte_enb_ul_fft(&uplinkConfig, signal);

	DecodeDataChannel(&uplinkConfig);	
}


int readSignal(cf_t* sig, unsigned int maxSymbolsNum, const char *filename)
{
	FILE* file = fopen(filename, "rb");
	int readBytes;

	readBytes = fread(sig, sizeof(cf_t), maxSymbolsNum, file);

	fclose(file);

	return readBytes;
}

int main(int argc, char** argv)
{
	char *filename = "C:\\Users\\rimon.orni\\Dropbox (Vayyar)\\Magic box\\Mars\\Board Test\\PUSCH Demod\\lte_fdd_UL_10ms_11p52MHz.bin";
	if (argc > 1)
	{
		filename = argv[1];
	}
	const int maxSigLen = 115200;
	cf_t *pBuff = (cf_t*)malloc(maxSigLen * sizeof(cf_t));
	readSignal(pBuff, maxSigLen, filename);

	cf_t* distances;
	cf_t* CEs;
	short* LLRs;
	unsigned int resultSize;

	CalculateEVM(pBuff, maxSigLen, &resultSize, &distances, &CEs, &LLRs);

	printf("ResultSize: %d\n", resultSize);
	for (int i = 0; i < 50; i++)
	{
		printf("LLRs[%d] = %d\n", i, LLRs[i]);
	}
	printf("\n\n");
	for (int i = 0; i < 50; i++)
	{
		printf("distances[%d] = %f + %f I\n", i, __real__ distances[i], __imag__ distances[i]);
	}
	printf("\n\n");
	for (int i = 0; i < resultSize - 3; i += 600)
	{
		printf("CEs[%d] = %f + %f I\n", i, __real__ CEs[i], __imag__ CEs[i]);
		printf("CEs[%d] = %f + %f I\n", i + 1, __real__ CEs[i + 1], __imag__ CEs[i + 1]);
		printf("CEs[%d] = %f + %f I\n", i + 2, __real__ CEs[i + 2], __imag__ CEs[i + 2]);
		printf("\n");
	}

	char *distFilename = "C:\\temp\\LTE_Distances.txt";
	char *ceFilename = "C:\\temp\\LTE_CEs.txt";
	FILE* distFile = fopen(distFilename, "w");
	FILE* ceFile = fopen(ceFilename, "w");

	for (int i = 0; i < resultSize; i++)
	{
		fprintf(distFile, "%f + %f I\n", __real__ distances[i], __imag__ distances[i]);
		fprintf(ceFile, "%f + %f I\n", __real__ CEs[i], __imag__ CEs[i]);
	}
}

