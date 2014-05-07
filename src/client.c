#include "common.h"

int main(int argc, char ** argv)
{
  int sc_fifo_fd, cs_fifo_fd;
  int i;
  FILE *fp;
  char rsa_tmp[130];
  ssize_t msg_size;
  uint8_t * buff;
  char * client_name;
  uint8_t bin_r[6];
  uint8_t bin_k[8];
  //uint8_t msg[ENCRYPTED_MSG_MAX + 20];
  unsigned char message[ENCRYPTED_MSG_MAX + 20];  //128+20
  char message_hex[(ENCRYPTED_MSG_MAX + 20) * 2]; //(128+20)*2
  uint8_t symm_cipher, hash, asymm_cipher;
  char ciphersuite;

 BIGNUM
    *CN, *CD, /* client private key */
    *SN, *SE, /* server public key */
    *r,   /* random number */
    *rc,  /* encrypted r */
    *key;   /* symmetric key */

  CN = BN_new();
  CD = BN_new();
  SN = BN_new();
  SE = BN_new();
  r = BN_new();
  rc = BN_new(); 
  key = BN_new();

	/* Mandatory arguments */
	if( !argv[1] || !argv[2] || !argv[3] || !argv[4] ) {
		fprintf(stderr,"client [server->client fifo] [client->server fifo] [password file] [username]\n");
		exit(1);
	}
         client_name = argv[4];


	/* Create connection with the server */
	fprintf(stderr,"Create connection...\n");
	sc_fifo_fd = open_channel(argv[1]);
	cs_fifo_fd = open_channel(argv[2]);

	write_msg(cs_fifo_fd,(const u_int8_t *)CONNECTION_STRING,strlen(CONNECTION_STRING));

	/* Read OK */
	if( read_string(sc_fifo_fd,OK_STRING) < 0 ) {
		fprintf(stderr,"Communication error\n");
		goto next1;
	}

   /* Server authentication */	
   /* GET public rsa key of S, (s_puk,n), from "client_folder/server_rsa64_public_key.txt" */
  char* filename = "client_folder/server_rsa_public_key.txt";
  int status = readRsaKey(filename, SN, SE);
  if(status ==-1){
   fprintf(stderr, "Error while reading server RSA public key...\n");
     goto next1;
  }
  /* CREATE a random number r */
  do {  
    fprng(bin_r, 6*8);    
    BN_bin2bn(bin_r, 6, r);
  } while (BN_is_zero(r) || BN_is_one(r));

  /* ENCRYPT r using (s_puk,n) -> c = r^s_puk mod n */
  BN_copy(rc, r);
  encrypt_msg(rc, SE, SN);

  /* WRITE c to S */
  buff = (uint8_t *) BN_bn2hex(rc);
  msg_size = strlen((char*)buff);
  if ((write_msg(cs_fifo_fd, buff, msg_size)) < 0) {
    fprintf(stderr, "Error while sending C to the server...\n");
    goto next1;
  }
  OPENSSL_free(buff);

  // READ r' from C
  /* ... */
  if ((msg_size = read_msg(sc_fifo_fd, &buff)) < 0) {
    fprintf(stderr, "Error while getting response to my challange from server...\n");
    goto next1;
  }

  buff[msg_size] = '\0';
  BN_hex2bn(&rc, (char*)buff);   /* overwrite rc to store r' */

  /* CHECK if r = r' */
  if (BN_cmp(r, rc) != 0) {
    fprintf(stderr, "Server authentication failed, closing.\n");
    goto next1;
  }

   /* Client authentication */
  // SEND client_name to S
  msg_size = strlen(client_name);
  if ((write_msg(cs_fifo_fd, (uint8_t*)client_name, msg_size)) < 0) {
    fprintf(stderr, "Error while sending name to the server...\n");
    goto next1;
  }
  /* GET private rsa key of C, (s_prk,n) from "client_folder/client_rsa64_private_key.txt" */
   char* prfilename = "client_folder/client_rsa_private_key.txt";
   status = readRsaKey(prfilename, CN, CD);
  if(status ==-1){
   fprintf(stderr, "Error while opening private key...\n");
     goto next1;
  }

  /* READ c from S */
  if ((msg_size = read_msg(sc_fifo_fd, &buff)) < 0) {
    fprintf(stderr, "Error while getting challenge from server...\n");
    goto next1;
  }

  buff[msg_size] = '\0';
  BN_hex2bn(&rc, (char*)buff);

  /* DECRYPT c using (c_prk,n) -> r' = c^c_prk mod n */
  decrypt_msg(rc, CD, CN);

  /* WRITE r' to S */
  buff = (uint8_t*) BN_bn2hex(rc);
  msg_size = strlen((char*)buff);
  if ((write_msg(cs_fifo_fd, buff, msg_size)) < 0) {
    fprintf(stderr, "Error while sending response to the server's challenge...\n");
    goto next1;
  }
  OPENSSL_free(buff);
  
  /* Negotiation of the cipher suite */
  /* ... */    
  if ((fp = fopen("client_folder/client_cipher_suite.txt", "r")) == NULL) {
    fprintf(stderr, "Error while reading cipher suite...\n");
  }
  fscanf(fp, "%c", &ciphersuite);
  fclose(fp);
  cipher_suite_table(ciphersuite, &symm_cipher, &hash, &asymm_cipher);

  /* SEND my cipher suite to server */
  if ((write_msg(cs_fifo_fd, (uint8_t*)(&ciphersuite), 1)) < 0) {
    fprintf(stderr, "Error while sending siphersuite to the server...\n");
    goto next1;
  }

  /* Negotiation of the private key */  
  /* ... */    
 if ((msg_size = read_msg(sc_fifo_fd, &buff)) < 0) {
    fprintf(stderr, "Error while getting symmetric key from server...\n");
    goto next1;
  }
  buff[msg_size] = '\0';
  BN_hex2bn(&key, (char*)buff);   /* overwrite rc to store r' */
 
  /* compute k from h and my private key */
  decrypt_msg(key, CD, CN);
  BN_bn2bin(key, bin_k);
   
  printf("\nKey exchange ends -------------------\n");

  /* Encrypt communication */
  if ((fp = fopen("client_folder/client_message.txt", "r")) == NULL) {
    fprintf(stderr, "Error while opening client message ...\n");
    goto next1;
  }
  fgets((char *) message, ENCRYPTED_MSG_MAX, fp);
  fclose(fp);
  

  msg_size = strlen((char *)message);
  uint8_t aclen = strlen((char *)message);  
 // message[aclen] = '\0';
  uint8_t hashed[20]={0};  
   /* hash the message */
  sponge_hash((uint8_t*)message, msg_size, (uint8_t*)(message + msg_size));
 
 
    enc_msg(symm_cipher, (uint8_t *)message,(const int)msg_size, bin_k);  // bin_k[6]
   for (i=0; i<msg_size+20; i++) {
     sprintf(message_hex+(2*i), "%02x", message[i]);
   }
   /*
  for (i=0; i<20; i++) {
     sprintf(message_hex+(2*(i+msg_size)), "%02x", hashed[i]);
   }*/
  /* send message + hash to server*/
  if ((write_msg(cs_fifo_fd, (uint8_t*)message_hex,(msg_size + 20)*2)) < 0) {
    fprintf(stderr, "Error while sending encrypted message to the server...\n");
    goto next1;
  }
  //read ack
   if (read_string(sc_fifo_fd, ACK_MSG) < 0) {
    fprintf(stderr,"ERROR: reading server acknowledgement!\n");
   } 
  /* Disconnection */ 

next1: // to be used when server writes the BYE string
	/* Close connection with the server */
  fprintf(stderr,"Closing connection...\n");

  /* Expect BYE */
  if (read_string(sc_fifo_fd, CLOSE_CONNECTION_STRING) < 0) {
    fprintf(stderr,"Communication error\n");
    goto next1;
  }

  close_channel(cs_fifo_fd);
  close_channel(sc_fifo_fd);

  BN_free(r);
  BN_free(rc);
  BN_free(CN);
  BN_free(CD);
  BN_free(SN);
  BN_free(SE);
  BN_free(key);

return 0;
}
