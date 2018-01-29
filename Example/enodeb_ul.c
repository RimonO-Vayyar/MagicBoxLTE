
#include "srslte/srslte.h"
#include <memory.h>
#include "srslte/utils/debug.h"

void ReadConfiguration(srslte_cell_t* cell,
		srslte_enb_ul_t* uplinkConfig,
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

  // PDSCH configuration
  //sib2->rr_config_common_sib.pdsch_cnfg.p_b = 0;
  //sib2->rr_config_common_sib.pdsch_cnfg.rs_power = -5;

  // PUSCH configuration
  //puschConfig->n_sb = 1;
  //puschConfig->hopping_mode = LIBLTE_RRC_HOPPING_MODE_INTER_SUBFRAME;
  //sib2->rr_config_common_sib.pusch_cnfg.pusch_hopping_offset = 4;
  //sib2->rr_config_common_sib.pusch_cnfg.enable_64_qam = false;
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

int DecodeDataChannel(srslte_enb_ul_t uplinkConfig)
{
	srslte_uci_data_t readData;
	memset(&readData, 0, sizeof(srslte_uci_data_t));
	srslte_enb_ul_pusch_t grants;
  
	srslte_softbuffer_rx_t softbuffer;

	// Copy grant info
	grants.rnti_idx = 0;
	memset(&grants.grant,    0, sizeof(srslte_ra_dl_dci_t));
	memset(&grants.location, 0, sizeof(srslte_dci_location_t));
	grants.grant.mcs_idx = 6;
	grants.softbuffer = &softbuffer;
	srslte_softbuffer_rx_init(grants.softbuffer, uplinkConfig.cell.nof_prb);

	grants.data = NULL;

     
	int res = -1;

	// 150 Mbps?
	grants.data = malloc((1024/8) * 150);

	srslte_ra_ul_dci_t dci; // Downlink Control Information
	dci.freq_hop_fl = -1;
	dci.type2_alloc.L_crb = 50;
	dci.type2_alloc.RB_start = 0;
	dci.mcs_idx = 6;

	srslte_ra_ul_grant_t ulGrant;

	if (srslte_ra_ul_dci_to_grant(&dci, uplinkConfig.cell.nof_prb, 0, &ulGrant, 0) != SRSLTE_SUCCESS)
	{
		printf("Error calculating the pusch grant\n");
		//return SRSLTE_ERROR; 
	}

	res = srslte_enb_ul_get_pusch(&uplinkConfig, &ulGrant, grants.softbuffer,
		grants.rnti_idx, grants.grant.rv_idx, grants.current_tx_nb, grants.data,
		&readData, 0);

	fprintf(stderr, "DecodeDataChannel returning: %d\n", res);

	return res;
}


int readSignal(cf_t* sig, unsigned int maxSymbolsNum)
{
	// FILE* file = fopen("d:\\Rimon.Orni\\Vayyar\\srsLTE_1.4\\lte_fdd_80ms_40M.bin", "rb");
	FILE* file = fopen("C:\\Users\\rimon.orni\\Dropbox (Vayyar)\\Magic box\\Mars\\Board Test\\PUSCH Demod\\lte_fdd_UL_10ms_11p52MHz.bin", "rb");
	float currReal, currImg;
	int readBytes;
	int index = 0;

	fread(sig, sizeof(cf_t), maxSymbolsNum, file);

	//do
	//{
	//	readBytes = fscanf(file, "%f, %f,\n", &currReal, &currImg);

	//	if (readBytes == 2)
	//	{
	//		sig[index] = currReal + currImg * _Complex_I;
	//		index++;
	//	}
	//} while ((readBytes == 2) && index < maxSymbolsNum);

	fclose(file);

	//FILE* binFile = fopen("C:\\Users\\rimon.orni\\Dropbox (Vayyar)\\Magic box\\Mars\\Board Test\\PUSCH Demod\\lte_fdd_UL_10ms_11p52MHz.bin", "wb");
	//fwrite(sig, sizeof(cf_t), maxSymbolsNum, binFile);

	//fclose(binFile);
	
	return index;
}

int main(int argc, char** argv)
{
	srslte_cell_t cell;
	srslte_enb_ul_t uplinkConfig;
	srslte_refsignal_dmrs_pusch_cfg_t puschConfig;
	srslte_pusch_hopping_cfg_t hoppingConfig;
	srslte_pucch_cfg_t pucchConfig;
	srslte_prach_cfg_t prach_cfg;

	//PRINT_DEBUG;

	ReadConfiguration(&cell, &uplinkConfig, &prach_cfg, &puschConfig, &hoppingConfig, &pucchConfig);

	if (srslte_enb_ul_init(&uplinkConfig, cell, &prach_cfg, &puschConfig, &hoppingConfig, &pucchConfig, 1))
	{
		printf("Failed to init uplink\n");
		return -1;
	}

	// TODO: Figure out what these magic numbers do
	srslte_pucch_set_threshold(&uplinkConfig.pucch, 1, 0);
	// srslte_sch_set_max_noi(&uplinkConfig.pusch.ul_sch, phy->params.pusch_max_its);

	// mac_interface_phy *mac = phy->mac; 

	// TODO: Read the UL signal from file here
	//const int maxSigLen = 16762500;
	const int maxSigLen = 115200;
	cf_t *pBuff = (cf_t*)malloc(maxSigLen * sizeof(cf_t));
	readSignal(pBuff, maxSigLen);
	srslte_enb_ul_cfg_rnti(&uplinkConfig, 0, 1);

	// Perform fft on the signal
	srslte_enb_ul_fft(&uplinkConfig, pBuff);

	DecodeDataChannel(uplinkConfig);

	int x = 4;
	x++;
}
