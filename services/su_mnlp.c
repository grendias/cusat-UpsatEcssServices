#include "su_mnlp.h"

#undef __FILE_ID__
#define __FILE_ID__ 3

mnlp_response_science_header flight_data;

//science_unit_script_inst su_scripts[1];

struct _MNLP_data MNLP_data;

//science_unit_script_inst su_scripts[SU_MAX_SCRIPTS_POPU] = { 
//    { .valid = false },
//    { .valid = false },
//    { .valid = false },
//    { .valid = false },
//    { .valid = false },
//    { .valid = false },
//    { .valid = false }
//};

/*174 response data + 22 for obc extra header and */
uint8_t su_inc_buffer[197];//198

/*the state of the science unit*/
SU_state su_state;

/*su scheduler various calling states*/
SAT_returnState tt_call_state;
SAT_returnState ss_call_state;
SAT_returnState scom_call_state;

MS_sid active_script;

/*points to the current start byte of a time's table begining, into the loaded script*/
uint16_t current_tt_pointer;

/*points to the current start byte of a script's sequence begining, into the loaded script*/
uint16_t current_ss_pointer;

SAT_returnState su_incoming_rx() {

    uint16_t size = 0;
    SAT_returnState res;
    res = HAL_su_uart_rx();
    if( res == SATR_EOT ) {
        
        if( su_inc_buffer[23] == SU_ERR_RSP_ID ) {
            event_crt_pkt_api(uart_temp, "SU_ERR", 969,969, "", &size, SATR_OK);
            HAL_uart_tx(DBG_APP_ID, (uint8_t *)uart_temp, size);
        }
        else if( su_inc_buffer[23] == OBC_SU_ERR_RSP_ID ) {
            event_crt_pkt_api(uart_temp, "SU_ERR_", 696,696, "", &size, SATR_OK);
            HAL_uart_tx(DBG_APP_ID, (uint8_t *)uart_temp, size);
        }
        else{
            event_crt_pkt_api(uart_temp, "SU_RESP", 696,696, "", &size, SATR_OK);
            HAL_uart_tx(DBG_APP_ID, (uint8_t *)uart_temp, size);
        }
        
//        if(su_scripts[(uint8_t) active_script - 1].rx_cnt < SU_SCI_HEADER + 5) { 
//            su_inc_buffer[ su_scripts[(uint8_t) active_script - 1].rx_cnt++] = c; }
//        if(su_scripts[(uint8_t) active_script - 1].rx_cnt < SU_SCI_HEADER + SU_RSP_PCKT_SIZE) { 
//            su_scripts[(uint8_t) active_script - 1].rx_buf[obc_su_scripts.rx_cnt++] = c; }
//        else {
//            su_scripts.rx_cnt = SU_SCI_HEADER;
//            su_scripts.timeout = time_now();
//            if(su_scripts.rx_buf[SU_SCI_HEADER] == SU_ERR_RSP_ID) { su_power_ctrl(P_RESET); }
//            
//        if(su_scripts[(uint8_t) active_script - 1].rx_cnt < SU_SCI_HEADER + 5) { 
//            su_scripts[(uint8_t) active_script - 1].rx_buf[obc_su_scripts.rx_cnt++] = c; }
//        if(su_scripts[(uint8_t) active_script - 1].rx_cnt < SU_SCI_HEADER + SU_RSP_PCKT_SIZE) { 
//            su_scripts[(uint8_t) active_script - 1].rx_buf[obc_su_scripts.rx_cnt++] = c; }
//        else {
//            su_scripts.rx_cnt = SU_SCI_HEADER;
//            su_scripts.timeout = time_now();
//            if(su_scripts.rx_buf[SU_SCI_HEADER] == SU_ERR_RSP_ID) { su_power_ctrl(P_RESET); }
//            /*science header*/
//            cnv32_8(time_now(), &su_scripts.rx_buf[0]);
//            cnv16_8(flight_data.roll, &su_scripts.rx_buf[4]);
//            cnv16_8(flight_data.pitch, &su_scripts.rx_buf[6]);
//            cnv16_8(flight_data.yaw, &su_scripts.rx_buf[8]);
//            cnv16_8(flight_data.roll_dot, &su_scripts.rx_buf[10]);
//            cnv16_8(flight_data.pitch_dot, &su_scripts.rx_buf[12]);
//            cnv16_8(flight_data.yaw_dot, &su_scripts.rx_buf[14]);
//            cnv16_8(flight_data.x_eci, &su_scripts.rx_buf[16]);
//            cnv16_8(flight_data.y_eci, &su_scripts.rx_buf[18]);
//            cnv16_8(flight_data.z_eci, &su_scripts.rx_buf[20]);
//
//            uint16_t size = SU_MAX_RSP_SIZE;
//           // mass_storage_storeLogs(SU_LOG, su_scripts.rx_buf, &size);

            /*science header*/
            cnv32_8( time_now(), &su_inc_buffer[0]);
            cnv16_8(flight_data.roll, &su_inc_buffer[4]);
            cnv16_8(flight_data.pitch, &su_inc_buffer[6]);
            cnv16_8(flight_data.yaw, &su_inc_buffer[8]);
            cnv16_8(flight_data.roll_dot, &su_inc_buffer[10]);
            cnv16_8(flight_data.pitch_dot, &su_inc_buffer[12]);
            cnv16_8(flight_data.yaw_dot, &su_inc_buffer[14]);
            cnv16_8(flight_data.x_eci, &su_inc_buffer[16]);
            cnv16_8(flight_data.y_eci, &su_inc_buffer[18]);
            cnv16_8(flight_data.z_eci, &su_inc_buffer[20]);
            //uint16_t t = 35000;
            //cnv16_8( t, &su_inc_buffer[20]);
            uint16_t size = SU_LOG_SIZE;
            mass_storage_storeFile(SU_LOG, 0 ,su_inc_buffer, &size);
//        }
    }
    return SATR_OK;
}

uint8_t time_lala = 5; //keep for fun and profit

void su_INIT(){

    su_state = su_off;
    su_load_scripts();
    for (MS_sid i = SU_SCRIPT_1; i <= SU_SCRIPT_7; i++) {
        
        su_populate_header( &(MNLP_data.su_scripts[(uint8_t) i - 1].scr_header), 
                            MNLP_data.su_scripts[(uint8_t) i - 1].file_load_buf);
        /*sort the scripts by increasing T_STARTTIME field (?)*/
    }
}

void su_load_scripts(){
//                                      SU_SCRIPT_7, SU_SCRIPT_6
    for( MS_sid i = SU_SCRIPT_1; i <= SU_SCRIPT_7; i++) {
        /*mark every script as non-valid, to check it later on memory and mark it valid*/
        MNLP_data.su_scripts[(uint8_t) i - 1].valid_str = false;
        /*mark every script as non-active*/
//        su_scripts[(uint8_t) i-1].active = false;
        /*load scripts on memory but, parse them at a later time, in order to unlock access to the storage medium for other users*/

        SAT_returnState res = mass_storage_su_load_api( i, MNLP_data.su_scripts[(uint8_t) i - 1].file_load_buf);
//        SAT_returnState res = mass_storage_su_load_api( SU_SCRIPT_6, MNLP_data.su_scripts[(uint8_t) i - 1].file_load_buf);
//        SAT_returnState res = SATR_OK;
        if( res == SATR_ERROR || res == SATR_CRC_ERROR){
            /*script is kept marked as invalid.*/
            //su_scripts[(uint8_t)i-1].valid = false;
            continue;
        }
        else{
            /* Mark the script as both structural/logical valid, to be parsed afterwards. */
            /* This is the first load from memory, maybe after a reset, so we assume that scripts are
             * logically valid. 
             */
            MNLP_data.su_scripts[(uint8_t) i - 1].valid_str = true;
            MNLP_data.su_scripts[(uint8_t) i - 1].valid_logi = true;
        }
    }
}

void su_SCH(){

    if( su_state == su_off || su_state == su_idle) {

        for( MS_sid i = SU_SCRIPT_1; i <= SU_SCRIPT_7; i++) {
            
            if( MNLP_data.su_scripts[(uint8_t) i - 1].valid_str == true && 
                MNLP_data.su_scripts[(uint8_t) i - 1].valid_logi == true &&
                MNLP_data.su_scripts[(uint8_t) i - 1].scr_header.start_time >= time_lala &&
                //TODO: add a check here for the following.
                /*this script has not been executed on this day &&*/
                MNLP_data.su_scripts[(uint8_t) i - 1].scr_header.start_time != 0) {

                //TODO: check here on non-volatile mem that we are not executing the same script on the same day,
                //due to a reset.
                //if a reset occured when we have executed a script from start to end, then on the next boot,
                //we will execute it again, we don't want that.
                su_state = su_running;
                active_script = (MS_sid) i;
                
                /* the first byte after the 12-byte sized script header,
                 * points to the first time table, on the active_script script.
                 */
                current_tt_pointer = SU_TT_OFFSET;
                /*finds the next tt that needs to be executed, it iterates all the tt to find the correct one*/
                for( uint16_t b = current_tt_pointer; 
                              b < SU_MAX_FILE_SIZE; b++) { //TODO: check until when for will run

                    /*if the script has been deleted/updated abort this old instance of it*/
                    if(MNLP_data.su_scripts[(uint8_t) i - 1].valid_logi != true ){ break;}
                    tt_call_state = 
                    polulate_next_time_table( MNLP_data.su_scripts[(uint8_t) i - 1].file_load_buf, 
                                              &MNLP_data.su_scripts[(uint8_t) i - 1].tt_header,
                                              &current_tt_pointer);
                    if( tt_call_state == SATR_EOT){
                        /*reached the last time table, go for another script, or this script, but on the NEXT day*/
                        //TODO: here to save on non-volatile mem that we have finished executing all ss'es, 
                        //see also line: 130-135
                        break;
                    }
                    else
                    if( tt_call_state == SATR_ERROR ){
                        /*non valid time table, go for next time table*/
//                            break;
                        continue;
                    }
                    else{
                        /*if the script has been deleted/updated abort this old instance of it*/
                        if(MNLP_data.su_scripts[(uint8_t) active_script - 1].valid_logi != true ){ break;}
                        /*call_state == SATR_OK*/
                        /*find the script sequence pointed by *time_table->script_index, and execute it */
                        /*start every search after current_tt_pointer, current_tt_pointer now points to the start of the next tt_header*/
                        current_ss_pointer = current_tt_pointer-1;
                        ss_call_state=
                        su_goto_script_seq( &current_ss_pointer, 
                                            &MNLP_data.su_scripts[(uint8_t) active_script - 1].tt_header.script_index);
                        if( ss_call_state == SATR_OK ){
//                            scom_call_state = ss_call_state;
//                            while( true ){ /*start executing commands in a script sequence*/
                            for(uint8_t p=0; p<SU_MAX_FILE_SIZE; p++){ /*start executing commands in a script sequence*/
                                
                                /*if the script has been deleted/updated abort this old instance of it*/
                                if(MNLP_data.su_scripts[(uint8_t) active_script - 1].valid_logi != true ){ break;}
                                scom_call_state =
                                su_next_cmd( MNLP_data.su_scripts[(uint8_t) active_script - 1].file_load_buf, 
                                             &MNLP_data.su_scripts[(uint8_t) active_script - 1].seq_header,
                                             &current_ss_pointer);
                                if( scom_call_state == SATR_EOT){
                                    /*no more commands on script sequences to be executed*/      
                                        /*go for the next command in the same script sequence*/
                                    /*reset seq_header->cmd_if field to something else other than 0xFE*/
                                    MNLP_data.su_scripts[(uint8_t) active_script - 1].seq_header.cmd_id = 0x0;
                                    break;
                                }
                                else
                                if( scom_call_state == SATR_ERROR){
                                    /*an unknown command has been encountered in the script sequences, so go for the next command*/
                                    continue;
                                }
                                else{
                                    /*handle script sequence command*/
                                    su_cmd_handler( &MNLP_data.su_scripts[(uint8_t) active_script - 1].seq_header  );
                                }
    //                        break;
//                            }//while ends here
                        }
                    }
                }//go for next time table
                //script handling for ends here, at this point every time table in (the current) script has been served.
                su_state = su_idle;
//                su_scripts[(uint8_t) active_script - 1].active = false;
            }//script run validity check if ends here
        }//go to check next script
    }
    
//    if (obc_su_scripts.state == su_running) {
//        if (obc_su_scripts.tt_header.time >= time_now()) {
//            if (su_scripts.tt_header.script_index == SU_SCR_TT_EOR) {
//                su_scripts.state = su_finished;
//            } else {
//                SAT_returnState res;
//                su_tt_handler(su_scripts.tt_header, su_scripts.scripts, su_scripts.active_script, &su_scripts.script_pointer_curr);
//                su_next_cmd(su_scripts.active_buf, &su_scripts.cmd_header, &su_scripts.script_pointer_curr);
//
//                for (uint16_t i = su_scripts.script_pointer_curr; i < SU_MAX_FILE_SIZE; i++) {
//
//                    res = su_cmd_handler(&su_scripts.cmd_header);
//                    if (res == SATR_EOT) {
//                        break;
//                    }
//                    su_next_cmd(su_scripts.active_buf, &su_scripts.cmd_header, &su_scripts.script_pointer_curr);
//                }
//                su_next_tt(su_scripts.active_buf, &su_scripts.tt_header, &su_scripts.tt_pointer_curr);
//
//            }
//            // }
//            //}
//            if (time_now() == midnight) {
//                su_INIT();
//                obc_su_scripts.state = su_idle;
//            }
//        }
//    }
}
}

SAT_returnState su_goto_script_seq(uint16_t *script_sequence_pointer, uint8_t *ss_to_go) {
    
    if( *ss_to_go == SU_SCR_TT_EOT ){
        /*if searching to go to script 0x55*/
        return SATR_EOT;
    }
    /*go until the end of the time tables entries*/
    while (MNLP_data.su_scripts[(uint8_t) active_script - 1].file_load_buf[(*script_sequence_pointer)++] !=
            /*su_scripts[(uint8_t) active_script - 1].tt_header.script_index*/ SU_SCR_TT_EOT) {
        //                            current_ss_pointer++;
    }/*now current_ss_pointer points to start of S1*/
    if( *ss_to_go == 0x41){ return SATR_OK;}
    
    /*an alternative implementation can count how many 0xFE we are passing over*/
    for (uint8_t i = 0x41; i <*ss_to_go; i++) {
        while (MNLP_data.su_scripts[(uint8_t) active_script - 1].file_load_buf[(*script_sequence_pointer)++] !=
                /*su_scripts[(uint8_t) active_script - 1].tt_header.script_index*/ SU_OBC_EOT_CMD_ID) {
            //                            current_ss_pointer++;
        }/*now current_ss_pointer points to start of S<times>*/
        /*move the pointer until end of 0xFE (SU_OBC_EOT_CMD_ID) command*/
        (*script_sequence_pointer) = (*script_sequence_pointer) + 2;
    }
    return SATR_OK;
}

SAT_returnState su_cmd_handler( science_unit_script_sequence *cmd) {

    HAL_sys_delay( ((cmd->dt_min * 60) + cmd->dt_sec) * 1000);
//    HAL_sys_delay( 1000 );
    uint8_t temp = 1;//to be removed
    
    if ( temp ==1 ){
    if( cmd->cmd_id == 0x05){
      HAL_su_uart_tx( cmd->command, cmd->len+1);
        
//        uint8_t su_out[105];
//        su_out[0]= 0x05;
//        su_out[1]= 0x63; //len
//        su_out[2]= 2; //seq_coun
//        HAL_UART_Transmit( &huart2, su_out, 102 , 10); //ver ok
//        HAL_su_uart_tx( su_out, 102);  
    }
//    else
//    if( cmd->cmd_id == 0x06 )
//    {
//        //don't send to hc sim, because it violates the state and leads to error dump.
//        return SATR_OK;
//    }
    else{
      HAL_su_uart_tx( cmd->command, cmd->len+2);
    }
    }
    else{
    //-------------------------------------------------------------------------
    if( cmd->cmd_id == SU_OBC_SU_ON_CMD_ID)         { su_power_ctrl(P_ON); } 
    else if(cmd->cmd_id == SU_OBC_SU_OFF_CMD_ID)    { su_power_ctrl(P_OFF); }
    else if(cmd->cmd_id == SU_OBC_EOT_CMD_ID)       { return SATR_EOT; } 
    else{
            if(cmd->cmd_id == 0x05)                     {
                HAL_su_uart_tx( cmd->command, cmd->len+1);
            }
            else
                HAL_su_uart_tx( cmd->command, cmd->len+2); 
    }
    }
//    HAL_sys_delay( 10);
//    if( cmd->cmd_id == 0x05){
//      HAL_su_uart_tx( cmd->command, cmd->len+1);
        
//        uint8_t su_out[105];
//        su_out[0]= 0x05;
//        su_out[1]= 0x63; //len
//        su_out[2]= 2; //seq_coun
//        HAL_UART_Transmit( &huart2, su_out, 102 , 10); //ver ok
//        HAL_su_uart_tx( su_out, 102);  
//    }
//    else
//    if( cmd->cmd_id == 0x06 )
//    {
//        //don't send to hc sim, because it violates the state and leads to error dump.
//        return SATR_OK;
//    }
//    else{
//      HAL_su_uart_tx( cmd->command, cmd->len+2);
//    }
//    HAL_su_uart_tx( &cmd->pointer, cmd->len);
//    HAL_su_uart_tx( &cmd->cmd_id, cmd->len);
    
//    if(cmd->cmd_id == SU_OBC_SU_ON_CMD_ID)          { su_power_ctrl(P_ON); } 
//    else if(cmd->cmd_id == SU_OBC_SU_OFF_CMD_ID)    { su_power_ctrl(P_OFF); } 
//    else if(cmd->cmd_id == SU_OBC_EOT_CMD_ID)       { return SATR_EOT; } 
//    else                                            { HAL_su_uart_tx( &cmd->pointer, cmd->len); }
//    HAL_sys_delay( 10);
    return SATR_OK;
}

SAT_returnState su_populate_header( science_unit_script_header *su_script_hdr, uint8_t *buf) {

    if(!C_ASSERT(buf != NULL && su_script_hdr != NULL) == true) { return SATR_ERROR; }
    
    union _cnv cnv;
    cnv.cnv8[0] = buf[0];
    cnv.cnv8[1] = buf[1];
    su_script_hdr->script_len = cnv.cnv16[0];

    cnv.cnv8[0] = buf[2];
    cnv.cnv8[1] = buf[3];
    cnv.cnv8[2] = buf[4];
    cnv.cnv8[3] = buf[5];
    su_script_hdr->start_time = cnv.cnv32;
    
    cnv.cnv8[0] = buf[6];
    cnv.cnv8[1] = buf[7];
    cnv.cnv8[2] = buf[8];
    cnv.cnv8[3] = buf[9];
    su_script_hdr->file_sn = cnv.cnv32;

    su_script_hdr->sw_ver = 0x1F & buf[10];    
    su_script_hdr->su_id = 0x03 & (buf[10] >> 5); //need to check this, seems ok
    su_script_hdr->script_type = 0x1F & buf[11];
    su_script_hdr->su_md = 0x03 & (buf[11] >> 5); //need to check this, seems ok

    cnv.cnv8[0] = buf[su_script_hdr->script_len-2]; /*to check?*/
    cnv.cnv8[1] = buf[su_script_hdr->script_len-1]; /*to check?*/
    su_script_hdr->xsum = cnv.cnv16[0];

    return SATR_OK;
}  
                                            
                                            /*a script , a temp buffer 2K*/
//SAT_returnState su_populate_scriptPointers( su_script *su_scr, uint8_t *buf) {
//
//    /*pointer to the first byte of a times-tables*/
//    uint16_t time_table_ptr = 0;
//    time_table_ptr = SU_TT_OFFSET;
//
//    science_unit_script_time_table temp_tt_header;
//    su_scr->header.scr_sequences = 0;
//    
//    for(uint16_t i = SU_TT_OFFSET; i < SU_MAX_FILE_SIZE; i++) {
//      
//                  /*temp buffer 2K, the tt_header_to fill, the pointer to the time_table*/
//        su_next_tt(buf, &temp_tt_header, &time_table_ptr); /*fill next time table structure*/
//        
//        if(temp_tt_header.script_index == SU_SCR_TT_EOT) { break; } /*EOT, 0x55 found*/
//        if(!C_ASSERT(time_table_ptr < su_scr->header.script_len) == true) { break; } /**/
//    }
//
//    su_scr->script_pointer_start[0] = time_table_ptr;
//    uint16_t script_temp = su_scr->script_pointer_start[0];
//
//    for(uint8_t a = 1; a < SU_CMD_SEQ; a++) {
//
//        if(script_temp == su_scr->header.script_len - 2) { break; }
//
//        for(uint16_t b = 0; b < SU_MAX_FILE_SIZE; b++) {
//            science_unit_script_sequence temp_cmd_header = {0}; /*initialize all struct elements to zero*/
//            if(su_next_cmd(buf, &temp_cmd_header, &script_temp) == SATR_ERROR) { break; };
//            if(temp_cmd_header.cmd_id == SU_OBC_EOT_CMD_ID && script_temp != su_scr->header.script_len - 2) { 
//                su_scr->script_pointer_start[a] = script_temp;
//                su_scr->header.scr_sequences++;
//                break; 
//            } else if(script_temp == su_scr->header.script_len - 2) { break; }
//        }
//        
//    }
//    if(!C_ASSERT(su_scr->header.scr_sequences < SU_CMD_SEQ) == true) { return SATR_ERROR; }
//
//    return SATR_OK;
//}

SAT_returnState polulate_next_time_table( uint8_t *file_buffer, science_unit_script_time_table *time_table, uint16_t *tt_pointer) {

    if(!C_ASSERT(file_buffer != NULL && time_table != NULL && tt_pointer != NULL) == true) { return SATR_ERROR; }
    if(!C_ASSERT(time_table->script_index != SU_SCR_TT_EOT) == true) { return SATR_EOT; }
    
    time_table->sec = file_buffer[(*tt_pointer)++];
    time_table->min = file_buffer[(*tt_pointer)++];
    time_table->hours = file_buffer[(*tt_pointer)++];
    time_table->script_index = file_buffer[(*tt_pointer)++];
    
    if(!C_ASSERT(time_table->sec < 59) == true) { return SATR_ERROR; }
    if(!C_ASSERT(time_table->min < 59) == true) { return SATR_ERROR; }
    if(!C_ASSERT(time_table->hours < 23) == true) { return SATR_ERROR; }
    if(!C_ASSERT(time_table->script_index == SU_SCR_TT_S1 || \
                 time_table->script_index == SU_SCR_TT_S2 || \
                 time_table->script_index == SU_SCR_TT_S3 || \
                 time_table->script_index == SU_SCR_TT_S4 || \
                 time_table->script_index == SU_SCR_TT_S5 || \
                 time_table->script_index == SU_SCR_TT_EOT) == true) { return SATR_ERROR; }

    return SATR_OK;
}

SAT_returnState su_next_cmd(uint8_t *file_buffer, science_unit_script_sequence *script_sequence, uint16_t *ss_pointer) {
    
//     for(uint8_t i = 0; i < 255; i++) {
//        script_sequence->command[i] = 254;
//     }
    
    if(!C_ASSERT(file_buffer != NULL && script_sequence != NULL && ss_pointer != NULL) == true) { return SATR_ERROR; }
    if(!C_ASSERT(script_sequence->cmd_id != SU_OBC_EOT_CMD_ID) == true) { return SATR_EOT; }

    script_sequence->dt_sec = file_buffer[(*ss_pointer)++];
    script_sequence->dt_min = file_buffer[(*ss_pointer)++];
    script_sequence->cmd_id = file_buffer[(*ss_pointer)++];
    script_sequence->command[0] = script_sequence->cmd_id;
//    script_sequence->pointer = *ss_pointer;
    script_sequence->len = file_buffer[(*ss_pointer)++];
    script_sequence->command[1] = script_sequence->len;
    //script_sequence->seq_cnt = file_buffer[(*ss_pointer)++];
    //script_sequence->command[2] = script_sequence->seq_cnt;
    
    //if( script_sequence->cmd_id == 0x05) { script_sequence->len = script_sequence->len + 2; }
    
    for(uint8_t i = 0; i < script_sequence->len; i++) {
        script_sequence->command[i+2] = file_buffer[(*ss_pointer)++]; 
    }

    if(!C_ASSERT(script_sequence->dt_sec < 59) == true) { return SATR_ERROR; }
    if(!C_ASSERT(script_sequence->dt_min < 59) == true) { return SATR_ERROR; }
    /*invert the C_ASSERT here, because the assertion handling function, suffers from a b.ov*/
    /*multiple assertions will be printed here until we reach the right command*/
    if(!C_ASSERT(script_sequence->cmd_id == SU_OBC_SU_ON_CMD_ID || \
                 script_sequence->cmd_id == SU_OBC_SU_OFF_CMD_ID || \
                 script_sequence->cmd_id == SU_RESET_CMD_ID || \
                 script_sequence->cmd_id == SU_LDP_CMD_ID || \
                 script_sequence->cmd_id == SU_HC_CMD_ID || \
                 script_sequence->cmd_id == SU_CAL_CMD_ID || \
                 script_sequence->cmd_id == SU_SCI_CMD_ID || \
                 script_sequence->cmd_id == SU_HK_CMD_ID || \
                 script_sequence->cmd_id == SU_STM_CMD_ID || \
                 script_sequence->cmd_id == SU_DUMP_CMD_ID || \
                 script_sequence->cmd_id == SU_BIAS_ON_CMD_ID || \
                 script_sequence->cmd_id == SU_BIAS_OFF_CMD_ID || \
                 script_sequence->cmd_id == SU_MTEE_ON_CMD_ID || \
                 script_sequence->cmd_id == SU_MTEE_OFF_CMD_ID || \
                 script_sequence->cmd_id == SU_OBC_EOT_CMD_ID) == true) { return SATR_ERROR; }

     return SATR_OK;

}

SAT_returnState su_power_ctrl(FM_fun_id fid) {

    tc_tm_pkt *temp_pkt = 0;

    function_management_pctrl_crt_pkt_api(&temp_pkt, EPS_APP_ID, fid, SU_DEV_ID);
    if(!C_ASSERT(temp_pkt != NULL) == true) { return SATR_ERROR; }

    route_pkt(temp_pkt);

    return SATR_OK;
}

//void su_timeout_handler(uint8_t error) {
//    
//    //cnv32_8(time_now(), &obc_su_scripts.rx_buf[0]);
//    //cnv16_8(flight_data.roll, &obc_su_scripts.rx_buf[4]);
//    //cnv16_8(flight_data.pitch, &obc_su_scripts.rx_buf[6]);
//    //cnv16_8(flight_data.yaw, &obc_su_scripts.rx_buf[8]);
//    //cnv16_8(flight_data.roll_dot, &obc_su_scripts.rx_buf[10]);
//    //cnv16_8(flight_data.pitch_dot, &obc_su_scripts.rx_buf[12]);
//    //cnv16_8(flight_data.yaw_dot, &obc_su_scripts.rx_buf[14]);
//    //cnv16_8(flight_data.x_eci, &obc_su_scripts.rx_buf[16]);
//    //cnv16_8(flight_data.y_eci, &obc_su_scripts.rx_buf[18]);
//    //cnv16_8(flight_data.z_eci, &obc_su_scripts.rx_buf[20]);
//
//    uint16_t buf_pointer = SU_SCI_HEADER;
//
//    su_scripts.rx_buf[buf_pointer++] = OBC_SU_ERR_RSP_ID;
//    buf_pointer++;
//    buf_pointer++;
//
//    su_scripts.rx_buf[buf_pointer++] = su_scripts.scripts[su_scripts.active_script].header.xsum;
//    cnv32_8(su_scripts.scripts[su_scripts.active_script].header.start_time, &su_scripts.rx_buf[buf_pointer++]);
//    buf_pointer += 4;
//    cnv32_8(su_scripts.scripts[su_scripts.active_script].header.file_sn, &su_scripts.rx_buf[buf_pointer++]);
//    buf_pointer += 4;
//    su_scripts.rx_buf[buf_pointer++] = (su_scripts.scripts[su_scripts.active_script].header.su_id << 5) | su_scripts.scripts[su_scripts.active_script].header.sw_ver;
//    su_scripts.rx_buf[buf_pointer++] = (su_scripts.scripts[su_scripts.active_script].header.su_md << 5) | su_scripts.scripts[su_scripts.active_script].header.script_type;
//
//    for(uint16_t i = SU_SCRIPT_1; i <= SU_SCRIPT_7; i++) {
//
//        su_scripts.rx_buf[buf_pointer++] = su_scripts.scripts[i].header.xsum;
//        cnv32_8(su_scripts.scripts[i].header.start_time, &su_scripts.rx_buf[buf_pointer++]);
//        buf_pointer += 4;
//        cnv32_8(su_scripts.scripts[i].header.file_sn, &su_scripts.rx_buf[buf_pointer++]);
//        buf_pointer += 4;
//        su_scripts.rx_buf[buf_pointer++] = (su_scripts.scripts[i].header.su_id << 5) | su_scripts.scripts[i].header.sw_ver;
//        su_scripts.rx_buf[buf_pointer++] = (su_scripts.scripts[i].header.su_md << 5) | su_scripts.scripts[i].header.script_type;
//    }
//
//    for(uint16_t i = 99; i < 173; i++) {
//        su_scripts.rx_buf[i] = 0;
//    }
//
//    uint16_t size = SU_MAX_RSP_SIZE;
//    mass_storage_storeLogs(SU_LOG, su_scripts.rx_buf, &size);
//    su_power_ctrl(P_RESET);
//    su_scripts.timeout = time_now();
//
//    su_next_tt(su_scripts.active_buf, &su_scripts.tt_header, &su_scripts.tt_pointer_curr);
//
//}