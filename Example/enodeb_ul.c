
#include "srslte/srslte.h"
#include <memory.h>

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
#if 0
  LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT msg[2];
  bzero(&msg[0], sizeof(LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT));
  bzero(&msg[1], sizeof(LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT));

  msg[0].N_sibs = 1;
  msg[0].sibs[0].sib_type = LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1;
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT *sib1 = &msg[0].sibs[0].sib.sib1;

  sib1->cell_id = 0x1234;
  sib1->tracking_area_code = 0x1234;
  sib1->freq_band_indicator = 2;
  sib1->N_plmn_ids = 1;
  sib1->plmn_id[0].id.mcc = 1;
  sib1->plmn_id[0].id.mnc = 1;
  sib1->plmn_id[0].resv_for_oper = LIBLTE_RRC_NOT_RESV_FOR_OPER;
  sib1->cell_barred = LIBLTE_RRC_CELL_NOT_BARRED;
  sib1->intra_freq_reselection = LIBLTE_RRC_INTRA_FREQ_RESELECTION_ALLOWED;
  sib1->q_rx_lev_min = -140;
  sib1->q_rx_lev_min_offset = 1;
  sib1->p_max = 10;
  sib1->p_max_present = true;
  sib1->si_window_length = LIBLTE_RRC_SI_WINDOW_LENGTH_MS40;
  sib1->N_sched_info = 1;
  sib1->sched_info[0].si_periodicity = LIBLTE_RRC_SI_PERIODICITY_RF16;
  sib1->sched_info[0].N_sib_mapping_info = 0;
  sib1->system_info_value_tag = 8;

  // Generate SIB2
  msg[1].N_sibs = 2;
  msg[1].sibs[0].sib_type = LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2;
  msg[1].sibs[1].sib_type = LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3;
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT *sib2 = &msg[1].sibs[0].sib.sib2;

  // RACH configuration
  sib2->rr_config_common_sib.rach_cnfg.num_ra_preambles = LIBLTE_RRC_NUMBER_OF_RA_PREAMBLES_N64;
  sib2->rr_config_common_sib.rach_cnfg.preambles_group_a_cnfg.present = false;
  sib2->rr_config_common_sib.rach_cnfg.preamble_init_rx_target_pwr = LIBLTE_RRC_PREAMBLE_INITIAL_RECEIVED_TARGET_POWER_DBM_N90;
  sib2->rr_config_common_sib.rach_cnfg.pwr_ramping_step = LIBLTE_RRC_POWER_RAMPING_STEP_DB6;
  sib2->rr_config_common_sib.rach_cnfg.preamble_trans_max = LIBLTE_RRC_PREAMBLE_TRANS_MAX_N10;
  sib2->rr_config_common_sib.rach_cnfg.ra_resp_win_size = LIBLTE_RRC_RA_RESPONSE_WINDOW_SIZE_SF10;
  sib2->rr_config_common_sib.rach_cnfg.mac_con_res_timer = LIBLTE_RRC_MAC_CONTENTION_RESOLUTION_TIMER_SF40;
  sib2->rr_config_common_sib.rach_cnfg.max_harq_msg3_tx = 4;

  // BCCH
  sib2->rr_config_common_sib.bcch_cnfg.modification_period_coeff = LIBLTE_RRC_MODIFICATION_PERIOD_COEFF_N16;

  // PCCH
  sib2->rr_config_common_sib.pcch_cnfg.default_paging_cycle = LIBLTE_RRC_DEFAULT_PAGING_CYCLE_RF128;
  sib2->rr_config_common_sib.pcch_cnfg.nB = LIBLTE_RRC_NB_ONE_THIRTY_SECOND_T;
#endif
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
#if 0
  // SRS configuration
  sib2->rr_config_common_sib.srs_ul_cnfg.present = false;

  // UL power control
  sib2->rr_config_common_sib.ul_pwr_ctrl.p0_nominal_pusch = -80;
  sib2->rr_config_common_sib.ul_pwr_ctrl.alpha = LIBLTE_RRC_UL_POWER_CONTROL_ALPHA_1;
  sib2->rr_config_common_sib.ul_pwr_ctrl.p0_nominal_pucch = -80;
  sib2->rr_config_common_sib.ul_pwr_ctrl.delta_flist_pucch.format_1  = LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1_0;
  sib2->rr_config_common_sib.ul_pwr_ctrl.delta_flist_pucch.format_1b = LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_1B_5;
  sib2->rr_config_common_sib.ul_pwr_ctrl.delta_flist_pucch.format_2  = LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2_2;
  sib2->rr_config_common_sib.ul_pwr_ctrl.delta_flist_pucch.format_2a = LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2A_2;
  sib2->rr_config_common_sib.ul_pwr_ctrl.delta_flist_pucch.format_2b = LIBLTE_RRC_DELTA_F_PUCCH_FORMAT_2B_2;
  sib2->rr_config_common_sib.ul_pwr_ctrl.delta_preamble_msg3 = 4;

  sib2->rr_config_common_sib.ul_cp_length = LIBLTE_RRC_UL_CP_LENGTH_1;

  sib2->ue_timers_and_constants.t300 = LIBLTE_RRC_T300_MS1000;
  sib2->ue_timers_and_constants.t301 = LIBLTE_RRC_T301_MS1000;
  sib2->ue_timers_and_constants.n310 = LIBLTE_RRC_N310_N10;
  sib2->ue_timers_and_constants.t311 = LIBLTE_RRC_T311_MS1000;
  sib2->ue_timers_and_constants.n311 = LIBLTE_RRC_N311_N1;

  sib2->time_alignment_timer = LIBLTE_RRC_TIME_ALIGNMENT_TIMER_INFINITY;
  sib2->additional_spectrum_emission = 1;
  sib2->arfcn_value_eutra.present = false;
  sib2->ul_bw.present = false;

  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT *sib3 = &msg[1].sibs[1].sib.sib3;

  bzero(sib3, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_3_STRUCT));
  sib3->q_hyst = LIBLTE_RRC_Q_HYST_DB_2;
  sib3->s_non_intra_search = 6;
  sib3->s_non_intra_search_present = true;
  sib3->thresh_serving_low = 4;
  sib3->cell_resel_prio = 6;
  sib3->q_rx_lev_min = -122;
  sib3->p_max = 23;
  sib3->p_max_present = true;
  sib3->s_intra_search = 10;
  sib3->s_intra_search_present = true;
  sib3->presence_ant_port_1 = true;
  sib3->neigh_cell_cnfg = 1;
  sib3->t_resel_eutra = 1;

  // Genreate payload
  LIBLTE_BIT_MSG_STRUCT bitbuffer[2];
  for (int i=0;i<2;i++) {
    liblte_rrc_pack_bcch_dlsch_msg(&msg[i], &bitbuffer[i]);
    srslte_bit_pack_vector(bitbuffer[i].msg, sib_buffer[i].msg, bitbuffer[i].N_bits);
    sib_buffer[i].N_bytes = (bitbuffer[i].N_bits-1)/8+1;
  }

  // Fill MAC scheduler configuration
  bzero(mac_cfg, sizeof(srsenb::sched_interface::cell_cfg_t));
  memcpy(&mac_cfg->cell, &cell, sizeof(srslte_cell_t));
  mac_cfg->sibs[0].len = sib_buffer[0].N_bytes;
  mac_cfg->sibs[0].period_rf = 8; // Fixed to 8 rf
  mac_cfg->sibs[1].len = sib_buffer[1].N_bytes;
  mac_cfg->sibs[1].period_rf = liblte_rrc_si_periodicity_num[sib1->sched_info[0].si_periodicity];
  mac_cfg->si_window_ms = liblte_rrc_si_window_length_num[sib1->si_window_length];


  mac_cfg->prach_rar_window = liblte_rrc_ra_response_window_size_num[sib2->rr_config_common_sib.rach_cnfg.ra_resp_win_size];

  // Copy PHY common configuration
  bzero(phy_cfg, sizeof(srsenb::phy_cfg_t));
  memcpy(&phy_cfg->cell, &cell, sizeof(srslte_cell_t));
  memcpy(&phy_cfg->prach_cnfg,  &sib2->rr_config_common_sib.prach_cnfg, sizeof(LIBLTE_RRC_PRACH_CONFIG_SIB_STRUCT));
  memcpy(&phy_cfg->pdsch_cnfg,  &sib2->rr_config_common_sib.pdsch_cnfg, sizeof(LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT));
  memcpy(&phy_cfg->pusch_cnfg,  &sib2->rr_config_common_sib.pusch_cnfg, sizeof(LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT));
  memcpy(&phy_cfg->pucch_cnfg,  &sib2->rr_config_common_sib.pucch_cnfg, sizeof(LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT));
  memcpy(&phy_cfg->srs_ul_cnfg, &sib2->rr_config_common_sib.srs_ul_cnfg, sizeof(LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT));
#endif
}

bool DecodeDataChannel(srslte_enb_ul_t uplinkConfig, srslte_enb_ul_pusch_t *grants, uint32_t transmissionTimeInterval)
{
	srslte_uci_data_t readData;
	memset(&readData, 0, sizeof(srslte_uci_data_t));
  
	uint16_t rntiIndex = 0;//grants->rnti_idx;
	srslte_softbuffer_rx_t softbuffer;

	for (uint32_t i = 0; i < 1; i++)
	{
	    // Copy grant info
		grants->rnti_idx = 0;
	    memset(&grants->grant,    0, sizeof(srslte_ra_dl_dci_t));
	    memset(&grants->location, 0, sizeof(srslte_dci_location_t));
	    grants->grant.mcs_idx = 6;
	    grants->softbuffer = &softbuffer;
		srslte_softbuffer_rx_init(grants->softbuffer, uplinkConfig.cell.nof_prb);

		grants->data = NULL;
	}

	//uint32_t wideband_cqi_value = 0;
  
      // Configure PUSCH CQI channel 
      //srslte_cqi_value_t cqi_value;
      //bool cqi_enabled = false; 
      //if (ue_db[rnti].cqi_en && srslte_cqi_send(ue_db[rnti].pmi_idx, tti_rx)) {
      //  cqi_value.type = SRSLTE_CQI_TYPE_WIDEBAND;
      //  cqi_enabled = true; 
      //} else if (grants[i].grant.cqi_request) {
      //  cqi_value.type = SRSLTE_CQI_TYPE_SUBBAND_HL;
      //  cqi_value.subband_hl.N = (phy->cell.nof_prb > 7) ? srslte_cqi_hl_get_no_subbands(phy->cell.nof_prb) : 0;
      //  cqi_enabled = true; 
      //}
      //if (cqi_enabled) {
      //  uci_data.uci_cqi_len = srslte_cqi_size(&cqi_value);
      //  Info("cqi enabled len=%d\n", uci_data.uci_cqi_len);
      //}
      
	srslte_ra_ul_grant_t ulGrant;
	int res = -1;

	// 150 Mbps?
	grants->data = malloc((1024/8) * 150);

	if (!srslte_ra_ul_dci_to_grant(&grants->grant, uplinkConfig.cell.nof_prb, 0, &ulGrant, transmissionTimeInterval % 8))
	{
		res = srslte_enb_ul_get_pusch(&uplinkConfig, &ulGrant, grants->softbuffer,
			grants->rnti_idx, grants->rv_idx, grants->current_tx_nb, grants->data,
			&readData, transmissionTimeInterval);
	} else {
		printf("Error calculating the pusch grant\n");
		return SRSLTE_ERROR; 
	}

      // mark this tti as having an ul grant to avoid pucch 
      //ue_db[rnti].has_grant_tti = tti_rx; 
      
      //srslte_ra_ul_grant_t phy_grant; 
      //int res = -1;
      
//      bool crc_res = (res == 0); 
                   
      // Save PHICH scheduling for this user. Each user can have just 1 PUSCH grant per TTI
      
	float snrInDb  = 10 * log10(srslte_chest_ul_get_snr(&uplinkConfig.chest));
      
      // Notify MAC new received data and HARQ Indication value
      //phy->mac->crc_info(tti_rx, rnti, phy_grant.mcs.tbs/8, crc_res);    
      //if (uci_data.uci_ack_len) {
      //  phy->mac->ack_info(tti_rx, rnti, uci_data.uci_ack && (crc_res || snr_db > PUSCH_RL_SNR_DB_TH));
      //}
      
      // Notify MAC of UL SNR and DL CQI 
      //if (snr_db >= PUSCH_RL_SNR_DB_TH) {
      //  phy->mac->snr_info(tti_rx, rnti, snr_db);
      //}
      //if (uci_data.uci_cqi_len>0 && crc_res) {
      //  phy->mac->cqi_info(tti_rx, rnti, wideband_cqi_value);
      //}
      
      // Save metrics stats 
      //ue_db[rnti].metrics_ul(phy_grant.mcs.idx, 0, snr_db, srslte_pusch_last_noi(&enb_ul.pusch));
	return SRSLTE_SUCCESS;
}

#if 0
// TODO: what is this for? uint32_t tti_rx
int DecodeControlChannel()
{
  uint32_t sf_rx = tti_rx%10;
  srslte_uci_data_t uci_data; 
  
  for(std::map<uint16_t, ue>::iterator iter=ue_db.begin(); iter!=ue_db.end(); ++iter) {
    uint16_t rnti = (uint16_t) iter->first;

    if (rnti >= SRSLTE_CRNTI_START && rnti <= SRSLTE_CRNTI_END && ue_db[rnti].has_grant_tti != (int) tti_rx) {
      // Check if user needs to receive PUCCH 
      bool needs_pucch = false, needs_ack=false, needs_sr=false, needs_cqi=false; 
      uint32_t last_n_pdcch = 0;
      bzero(&uci_data, sizeof(srslte_uci_data_t));
      
      if (ue_db[rnti].I_sr_en) {
        if (srslte_ue_ul_sr_send_tti(ue_db[rnti].I_sr, tti_rx)) {
          needs_pucch = true; 
          needs_sr = true; 
          uci_data.scheduling_request = true; 
        }
      }      
      if (phy->ack_is_pending(sf_rx, rnti, &last_n_pdcch)) {
        needs_pucch = true; 
        needs_ack = true; 
        uci_data.uci_ack_len = 1; 
      }
      srslte_cqi_value_t cqi_value;
      if (ue_db[rnti].cqi_en && (ue_db[rnti].pucch_cqi_ack || !needs_ack)) {
        if (srslte_cqi_send(ue_db[rnti].pmi_idx, tti_rx)) {
          needs_pucch = true; 
          needs_cqi = true; 
          cqi_value.type = SRSLTE_CQI_TYPE_WIDEBAND; 
          uci_data.uci_cqi_len = srslte_cqi_size(&cqi_value);
        }
      }
      
      if (needs_pucch) {
        if (srslte_enb_ul_get_pucch(&enb_ul, rnti, last_n_pdcch, sf_rx, &uci_data)) {
          fprintf(stderr, "Error getting PUCCH\n");
          return SRSLTE_ERROR; 
        }
        if (uci_data.uci_ack_len > 0) {
          phy->mac->ack_info(tti_rx, rnti, uci_data.uci_ack && (srslte_pucch_get_last_corr(&enb_ul.pucch) >= PUCCH_RL_CORR_TH));      
        }
        if (uci_data.scheduling_request) {
          phy->mac->sr_detected(tti_rx, rnti);                
        }
        
        char cqi_str[64];
        if (uci_data.uci_cqi_len) {
          srslte_cqi_value_unpack(uci_data.uci_cqi, &cqi_value);
          phy->mac->cqi_info(tti_rx, rnti, cqi_value.wideband.wideband_cqi);
          sprintf(cqi_str, ", cqi=%d", cqi_value.wideband.wideband_cqi);
        }
        log_h->info("PUCCH: rnti=0x%x, corr=%.2f, n_pucch=%d, n_prb=%d%s%s%s\n", 
                    rnti, 
                    srslte_pucch_get_last_corr(&enb_ul.pucch),
                    enb_ul.pucch.last_n_pucch, enb_ul.pucch.last_n_prb,
                    needs_ack?(uci_data.uci_ack?", ack=1":", ack=0"):"", 
                    needs_sr?(uci_data.scheduling_request?", sr=yes":", sr=no"):"", 
                    needs_cqi?cqi_str:"");                


        // Notify MAC of RL status 
        if (!needs_sr) {
          if (srslte_pucch_get_last_corr(&enb_ul.pucch) < PUCCH_RL_CORR_TH) {
            Debug("PUCCH: Radio-Link failure corr=%.1f\n", srslte_pucch_get_last_corr(&enb_ul.pucch));
            phy->mac->rl_failure(rnti);
          } else {
            phy->mac->rl_ok(rnti);
          }          
        }                
      }
    }
  }    
  return 0; 
}
#endif

int readSignal(cf_t* sig, unsigned int maxSymbolsNum)
{
	FILE* file = fopen("d:\\Rimon.Orni\\Vayyar\\srsLTE_1.4\\lte_fdd_80ms_40M.bin", "rb");

	float currReal, currImg;
	int readBytes = fread(&currReal, 1, sizeof(float), file);
	int index = 0;

	while (readBytes == sizeof(float) && index < maxSymbolsNum)
	{
		readBytes = fread(&currImg, 1, sizeof(float), file);

		if (readBytes < sizeof(float))
		{
			break;
		}

		sig[index] = currReal + currImg * _Complex_I;
		index++;
	}

	fclose(file);

	return index;
}

int main(int argc, void* argv)
{
	srslte_cell_t cell;
	srslte_enb_ul_t uplinkConfig;
	srslte_refsignal_dmrs_pusch_cfg_t puschConfig;
	srslte_pusch_hopping_cfg_t hoppingConfig;
	srslte_pucch_cfg_t pucchConfig;
	srslte_prach_cfg_t prach_cfg;

	cf_t* rx_buffer = srslte_vec_malloc(2 * sizeof(cf_t) * SRSLTE_SF_LEN_PRB(cell.nof_prb));
	if (rx_buffer == NULL) 
	{
		printf("Failed to allocate signal buffer\n");
		return -1; 
	}

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
	const int maxSigLen = 16762500;
	cf_t *pBuff = (cf_t*)malloc(maxSigLen * sizeof(cf_t));
	readSignal(pBuff, maxSigLen);
	srslte_enb_ul_cfg_rnti(&uplinkConfig, 0, 1);

	// Perform fft on the signal
	srslte_enb_ul_fft(&uplinkConfig, pBuff);
	srslte_enb_ul_pusch_t grants;

	uint32_t transmissionTimeInterval;
	DecodeDataChannel(uplinkConfig, &grants, transmissionTimeInterval);

	int x = 4;
	x++;
}
