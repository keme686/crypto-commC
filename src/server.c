#include "common.h"


int open_fifo(const char * pathname)
{
	/* Recreate the FIFO in pathname */
	unlink(pathname);
	if( mkfifo(pathname,0600) < 0 ) {
		perror("mkfifo()");
		exit(1);
	}

	/* Open the channel */
	return (open_channel(pathname));
}

int wait_connection(int channel_fd)
{
	ssize_t msg_size;
	u_int8_t * buff;

	/* Wait a connection from the client */
	if( 	((msg_size = read_msg(channel_fd,&buff)) < 0) ||
		(msg_size != strlen(CONNECTION_STRING) ||
		strncasecmp((const char *)CONNECTION_STRING,(const char *)buff,msg_size)) ) {
		/* Protocol error */
		return (-1);
	}

	return (0);
}


int main(int argc, char ** argv)
{
  int sc_fifo_fd, cs_fifo_fd;	
 
  char clientName[130], cipher_tmp, clientCipherSuite;

  int i, found;  
  unsigned int msg_size;  //size of message
  uint8_t *buff;  //message buffer 
  unsigned int tmp;
  
  uint8_t r[6];  //psudo-random number
  uint8_t k[6];  //key
  uint8_t encType, hashType, publicKeyType;  //encryption type, hashing type, and public key type
  int k_len;  //key length
  FILE *fp;

  BIGNUM *SN, *SD,  //server N and D
         *CE, *CN,  //Client E and N
         *bn_r, *bn_r1,
         *recievedMsg;  // reading numbers and randoms

  SN = BN_new();
  SD = BN_new();
  CE = BN_new();
  CN = BN_new();
  bn_r = BN_new();
  bn_r1 = BN_new();
  recievedMsg = BN_new();

	/* Mandatory arguments */
	if( !argv[1] || !argv[2] || !argv[3] ) {
		fprintf(stderr,"server [server->client fifo] [client->server fifo] [password file]\n");
		exit(1);
	}  
	/* Main loop */

     
main_loop:     

     		/* Open the server->client fifo */
		fprintf(stderr,"Opening server->client fifo...\n");
		sc_fifo_fd = open_fifo(argv[1]);

		/* Open the client->server fifo */
		fprintf(stderr,"Opening client->server fifo...\n");
		cs_fifo_fd = open_fifo(argv[2]);

		/* Wait for a connection */
		fprintf(stderr,"Waiting connection...\n");
		if( wait_connection(cs_fifo_fd) < 0 ) {
			fprintf(stderr,"Communication error...\n");
			goto next1;
		}
		
		/* Write OK */
		write_OK(sc_fifo_fd);
                                                
     /* Server authentication */
    // GET private rsa key of S, (s_prk,n) from "server_folder/server_rsa_private_key.txt"
    char* filename = "server_folder/server_rsa_private_key.txt";
    int status = readRsaKey(filename, SN, SD);
    if(status ==-1){
     fprintf(stderr, "Error while reading server RSA private key...\n");
     goto next1;
    }
    // READ c from C
   if ((msg_size = read_msg(cs_fifo_fd,&buff)) < 0) {
      fprintf(stderr, "Error while getting C from the client...\n");
      goto next1;
    }

    // DECRYPT recievedMsg using (s_prk,n) -> r' = c^s_prk mod n
    buff[msg_size] = '\0';
    BN_hex2bn(&recievedMsg, (const char *) buff);

    decrypt_msg(recievedMsg, SD, SN);  // recievedMsg = bn_r^SD mod SN
    buff = (uint8_t *) BN_bn2hex(recievedMsg);
    
    // SEND r' to C
     if ((write_msg(sc_fifo_fd, buff, strlen((char *) buff))) < 0) {
      fprintf(stderr, "Error while sending C to the client...\n");
      goto next1;
    }
    OPENSSL_free(buff);

   /* Client authentication */
    // READ client name nm of C
    /* ... */
    if ((msg_size = read_msg(cs_fifo_fd,&buff)) < 0) {
      fprintf(stderr, "Error while getting the client name...\n");
      goto next1;
    }
    buff[msg_size] = '\0';
    strncpy((char *)clientName, (const char *) buff, 130);  //maximum client name = 130
   
    /* GET the public rsa keys of the possible clients associated to each name,
     * (names[],c_puk[],n[]) from "client_folder/clients_rsa64_public_keys.txt"
     */
    /* EXTRACT from (names[],c_puk[],n[]) the pair (c_puk[i],n[i]) where names[i] = nm */
    char* pubfilename = "server_folder/clients_rsa_public_keys.txt";
    status = readClientsPublicRsaKey(pubfilename, clientName, CN, CE);
    if(status != 1)
     {
      goto next1;
     }   
     
    /* CREATE a pseudo-random message r */
    do {      
      fprng(r, 6*8);      
      BN_bin2bn(r, 6, bn_r);
    } while (BN_is_zero(bn_r) || BN_is_one(bn_r));    

    /* ENCRYPT r using c_puk[i] -> r' = r^c_puk[i] mod n[i] */
    encrypt_msg(bn_r, CE, CN);    

    /* WRITE c to C */
    buff = (uint8_t *) BN_bn2hex(bn_r);
    if ((write_msg(sc_fifo_fd, buff, strlen((char *) buff))) < 0) {
      fprintf(stderr, "Error while sending C to the client...\n");
      goto next1;
    }
    OPENSSL_free(buff);

    /* READ r' from C */
    if ((msg_size = read_msg(cs_fifo_fd,&buff)) < 0) {
      fprintf(stderr, "ERROR: while getting r' from the client...\n");
      goto next1;
    }
    
    /* CHECK that r = r' */
    BN_bin2bn(r, 6, bn_r);

    buff[msg_size] = '\0';
    BN_hex2bn(&bn_r1, (const char *) buff);    
    if (BN_cmp(bn_r, bn_r1) != 0) {
      fprintf(stderr, "ERROR: Client authentication failed! r and r' wasn't the same!\n");
      goto next1;
    }		

    /* Negotiation of the cipher suite */
   if ((msg_size = read_msg(cs_fifo_fd,&buff)) < 0) {
      fprintf(stderr, "ERROR: while getting the list from the client...\n");
      goto next1;
    }  
    clientCipherSuite = buff[0];
    cipher_suite_table(clientCipherSuite, &encType, &hashType, &publicKeyType);

    /* check cipher suite */
    if ((fp = fopen("server_folder/server_cipher_suite_list.txt", "r")) == NULL) {
      fprintf(stderr, "ERROR: while getting the cipher list...\n");
      goto next1;
    }
    found = 0;  //cipher suite found?
    while (!feof(fp)) {
      fscanf(fp, "%c", &cipher_tmp);
      if (clientCipherSuite == cipher_tmp) {
        found = 1;
        break;
      }
    }
    if (found == 0) {
      fprintf(stderr, "ERROR: client unrecognized\n");
      goto next1;
    }
    fclose(fp);

    /* Negotiation of the private key */  
    do {     
      if (encType == 1) {
        k_len = 3;   //bunny key 3*8 = 24 bits
      }
      else {
        k_len = 6;  //64 bit key
      }
      sprng(k, k_len*8);  //fast psudorandom generator
      BN_bin2bn(k, k_len, bn_r);
    } while (BN_is_zero(bn_r) || BN_is_one(bn_r));

    /* encrypt generated key using client public key */
    encrypt_msg(bn_r, CE, CN);      

    /* send key to C */
    buff = (uint8_t *) BN_bn2hex(bn_r);
    if ((write_msg(sc_fifo_fd, buff, strlen((char *) buff))) < 0) {
      fprintf(stderr, "ERROR: while sending C to the client...\n");
      goto next1;
    }
    OPENSSL_free(buff);

    printf("Key exchange ends -------------------\n");

    /* Encrypt communication */
     uint8_t c[MSG_SIZE_MAX]={0};
     int chlen=0, msg_hexlen=0;  //msg length, msg with hash length
      if ((msg_size = read_msg(cs_fifo_fd,&buff)) < 0) {
      fprintf(stderr, "ERROR: while reading message from the client...\n");
      goto next1;
    }

    msg_hexlen = msg_size - 20*2; //msg_size - hash length*2
    if (msg_hexlen <= 0) {
      fprintf(stderr, "ERROR: malformed message...\n");
      goto next1;
    }
     uint8_t recievedHash[20]={0}; //hash
     uint8_t computedHash[20]={0}; //hash
    chlen = msg_hexlen/2;
    for (i=0; i<msg_size; i+=2) {
      if (i < msg_hexlen) {
        sscanf((char *) (buff+i), "%02x", &tmp);
        c[i/2] = (uint8_t) tmp;  //cipher
      }
      else {
        sscanf((char *) (buff+i), "%02x", &tmp);
        recievedHash[(i - msg_hexlen)/2] = (uint8_t) tmp;  //hash
      }
    }  
    
    /* Decrypt C */
    decrypt_m(encType, c, chlen, k);  
  
     /* compute hash */
    sponge_hash(c, chlen, computedHash);
     c[chlen] = '\0';       
    
    found = 1;  //mismatch found?
    for (i=0; i<20; i++) {
      if (recievedHash[i] != computedHash[i]) {                
        found = 0;
      }
    }
    printf("fond: %d\n", found);
    /* If mismatch found print error */
    if (found == 0) {
      if ((write_msg(sc_fifo_fd, (uint8_t *) CORRUPTED_MSG, strlen(CORRUPTED_MSG))) < 0) {
        fprintf(stderr, "ERROR: while writing to the client...\n");
        goto next1;
      }
    }
   
    if ((fp = fopen("server_folder/received_messages.txt", "a+")) == NULL) {
        fprintf(stderr, "ERROR: while opening received message...\n");
        fclose(fp);
        goto next1;
    }
    fprintf(fp, "%s", c);  //write message to file
    fclose(fp);

    /* Send ack to C */
    if ((write_msg(sc_fifo_fd, (uint8_t *) ACK_MSG, strlen(ACK_MSG))) < 0) {
      fprintf(stderr, "ERROR: while writing C to the client...\n");
      goto next1;
    }
    /* Disconnection */       
    // to be used when server writes the BYE string
       /* Close current connection */
next1: 
        fprintf(stderr,"Closing connection...\n");
	write_BYE(sc_fifo_fd);
	close_channel(sc_fifo_fd);
	close_channel(cs_fifo_fd);
goto main_loop ;

 next2:   
         close_channel(sc_fifo_fd);
         close_channel(cs_fifo_fd);
         BN_free(SN);
         BN_free(SD);
         BN_free(CN);
         BN_free(CE);
         BN_free(bn_r);
         BN_free(bn_r1);
         BN_free(recievedMsg);
	exit(0);
}

