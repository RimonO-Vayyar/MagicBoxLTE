
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

void PrepareToDecode(srslte_enb_ul_t *uplinkConfig, cf_t* signal)
{
	srslte_cell_t cell;
	srslte_prach_cfg_t prach_cfg;
	srslte_refsignal_dmrs_pusch_cfg_t puschConfig;
	srslte_pusch_hopping_cfg_t hoppingConfig;
	srslte_pucch_cfg_t pucchConfig;

	SetConfiguration(&cell, &prach_cfg, &puschConfig, &hoppingConfig, &pucchConfig);

	if (srslte_enb_ul_init(uplinkConfig, cell, &prach_cfg, &puschConfig, &hoppingConfig, &pucchConfig, 1))
	{
		printf("Failed to init uplink\n");
		return;
	}

	unsigned int resultSize = uplinkConfig->pusch.max_re;

	uplinkConfig->pusch.q = srslte_vec_malloc(sizeof(int16_t) * resultSize * srslte_mod_bits_x_symbol(SRSLTE_MOD_QPSK));
	if (!uplinkConfig->pusch.q) {
		printf("Failed to allocate LLRs\n");
		return;
	}

	uplinkConfig->pusch.d = srslte_vec_malloc(sizeof(cf_t) * resultSize);
	if (!uplinkConfig->pusch.d) {
		printf("Failed to allocate distances\n");
		return;
	}

	uplinkConfig->pusch.ce = srslte_vec_malloc(sizeof(cf_t) * resultSize);
	if (!uplinkConfig->pusch.ce) {
		printf("Failed to allocate CEs\n");
		return;
	}

	srslte_pucch_set_threshold(&(uplinkConfig->pucch), 1, 0);

	srslte_enb_ul_cfg_rnti(uplinkConfig, 0, 1);
	
	// Perform fft on the signal
	srslte_enb_ul_fft(uplinkConfig, signal);
}

void CalculateEVM(cf_t* distances, double* EVMs, unsigned int resultSize)
{
	cf_t refSignalPos = sqrt(2) + 1 * I;
	cf_t refSignalNeg = -1 * sqrt(2) + 1 * I;

	for (unsigned int i = 0; i < resultSize; i++)
	{
		cf_t refSignal = (__real__ distances[i] > 0) ? refSignalPos : refSignalNeg;

		EVMs[i] = 100 * cabsf(distances[i] - refSignal);
	}
}


void DecodeDataChannel(cf_t* signal, const unsigned int signalLen, unsigned int* resultSize, cf_t** distances, cf_t** CEs, short** LLRs)
{
	srslte_ra_ul_dci_t      grant;
	srslte_dci_location_t   location;
	srslte_softbuffer_rx_t softbuffer;
	srslte_enb_ul_t uplinkConfig;

	PrepareToDecode(&uplinkConfig, signal);
	*resultSize = uplinkConfig.pusch.max_re;

	// Copy grant info
	memset(&grant, 0, sizeof(srslte_ra_dl_dci_t));
	memset(&location, 0, sizeof(srslte_dci_location_t));
	grant.mcs_idx = 6;
	srslte_softbuffer_rx_init(&softbuffer, uplinkConfig.cell.nof_prb);

	srslte_ra_ul_dci_t dci; // Downlink Control Information
	dci.freq_hop_fl = -1;
	dci.type2_alloc.L_crb = 50;
	dci.type2_alloc.RB_start = 0;
	dci.mcs_idx = 6;

	srslte_ra_ul_grant_t ulGrant;

	if (srslte_ra_ul_dci_to_grant(&dci, uplinkConfig.cell.nof_prb, 0, &ulGrant, 0) != SRSLTE_SUCCESS)
	{
		printf("Error calculating the pusch grant\n");
		return;
	}

	srslte_uci_data_t uci_data;
	memset(&uci_data, 0, sizeof(srslte_uci_data_t));
	uint8_t *data = malloc((1024 / 8) * 150);

	srslte_enb_ul_get_pusch(&uplinkConfig,
		&ulGrant,
		&softbuffer,
		0,           // rnti_idx
		grant.rv_idx,
		0,           // current_tx_nb,
		data,        // data
		&uci_data,
		0            // tti
	);

	*LLRs = uplinkConfig.pusch.q;
	*distances = uplinkConfig.pusch.d;
	*CEs = uplinkConfig.pusch.ce;

	return;
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
	double* EVMs;
	cf_t* CEs;
	short* LLRs;
	unsigned int resultSize;


	DecodeDataChannel(pBuff, maxSigLen, &resultSize, &distances, &CEs, &LLRs);
	printf("ResultSize: %d\n", resultSize);
	
	EVMs = srslte_vec_malloc(sizeof(double) * resultSize);
	if (!EVMs) {
		printf("Failed to allocate EVMs\n");
		exit(1);
	}
	CalculateEVM(distances, EVMs, resultSize);

	for (int i = 0; i < 50; i++)
	{
		printf("LLRs[%d] = %d\n", i, LLRs[i]);
	}
	printf("\n\n");
	for (int i = 0; i < 50; i++)
	{
		printf("EVMs[%d] = %f\n", i, EVMs[i]);
	}
	printf("\n\n");
	for (int i = 0; i < resultSize - 3; i += 600)
	{
		printf("CEs[%d] = %f + %f I\n", i, __real__ CEs[i], __imag__ CEs[i]);
		printf("CEs[%d] = %f + %f I\n", i + 1, __real__ CEs[i + 1], __imag__ CEs[i + 1]);
		printf("CEs[%d] = %f + %f I\n", i + 2, __real__ CEs[i + 2], __imag__ CEs[i + 2]);
		printf("\n");
	}

	char *EVMFilename = "C:\\temp\\LTE_EVMs.txt";
	char *ceFilename = "C:\\temp\\LTE_CEs.txt";
	FILE* EVMFile = fopen(EVMFilename, "w");
	FILE* ceFile = fopen(ceFilename, "w");

	for (int i = 0; i < resultSize; i++)
	{
		fprintf(EVMFile, "%f\n", EVMs[i]);
		fprintf(ceFile, "%f + %f I\n", __real__ CEs[i], __imag__ CEs[i]);
	}
}

