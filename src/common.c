#include "common.h"

void printBinary(uint64_t x, int d)
{
  char buffer[d+1];
  int index=0;
  while(d>0)
  {
   if(x&1)
     buffer[index++]='1';
   else 
     buffer[index++]='0';
   x>>=1;
   d--;
  }
  while(index>0)
   printf("%c", buffer[--index]);
   return;
}

void print_buff(u_int8_t * buff, size_t buff_size)
{
	int i = 0, j;

	while( i < buff_size ) {
		j = 0;
		while( ((j + i) < buff_size) && (j < 16) ) {
			fprintf(stderr,"%02X ",buff[i+j]);
			j++;
		}

		while( j < 16 ) {
			fprintf(stderr,"-- ");
			j++;
		}
		fprintf(stderr," *** ");

		j = 0;
		while( ((j + i) < buff_size) && (j < 16) ) {
			fprintf(stderr,"%c",isprint(buff[i+j]) ? buff[i+j] : '.');
			j++;
		}

		while( j < 16 ) {
			fputc('.',stderr);
			j++;
		}

    		i += j;
		fputc('\n',stderr);
	}
}

int write_OK(int channel_fd)
{
	return (write_msg(channel_fd,(const u_int8_t *)OK_STRING,strlen(OK_STRING)));
}

int write_BYE(int channel_fd)
{
	return (write_msg(channel_fd,(const u_int8_t *)CLOSE_CONNECTION_STRING,strlen(CLOSE_CONNECTION_STRING)));
}

int open_channel(const char * pathname)
{
	int fd;

	/* Open the FIFO in pathname */
	if( (fd = open(pathname,O_RDWR)) < 0 ) {
		perror("open()");
		exit(1);
	}

	return (fd);
}

void close_channel(int channel_fd)
{
	/* Close the file descriptor */
	close(channel_fd);
}

ssize_t read_msg(int channel_fd, u_int8_t ** msg)
{
	ssize_t read_len;
	u_int16_t msg_size;
	static u_int8_t msg_buff[MSG_SIZE_MAX];

	/* The first 16 bits represents the message size */
	if( read(channel_fd,(void *)&msg_size,sizeof(msg_size)) < 0 ) {
		perror("read()");
		exit(1);
	}

	fprintf(stderr,"Reading size:\n");
	print_buff((u_int8_t *)(&msg_size),sizeof(msg_size));

	/* Verify that the size of the message is no more than MSG_SIZE_MAX bytes */
	if( msg_size >= MSG_SIZE_MAX ) {
		fprintf(stderr,"Message too large\n");
		return (-1);
	}

	/* Read msg_size bytes from the channel */
	if( (read_len = read(channel_fd,(void *)msg_buff,msg_size)) < 0 ) {
		perror("read()");
		exit(1);
	}

	/* Assign the buffer to msg */
	(*msg) = msg_buff;

	fprintf(stderr,"Reading message:\n");
	print_buff((u_int8_t *)(*msg),msg_size);

	return (read_len);
}

ssize_t write_msg(int channel_fd, const u_int8_t * msg, u_int16_t msg_size)
{
	/* Send the size */
	if( write(channel_fd,(const void *)&msg_size,sizeof(msg_size)) < 0 ) {
		perror("write()");
		exit(1);
	}

	fprintf(stderr,"Writing size:\n");
	print_buff((u_int8_t *)&msg_size,sizeof(msg_size));

	/* Send the message */
	if( write(channel_fd,(const void *)msg,msg_size) < 0 ) {
		perror("write()");
		exit(1);
	}

	fprintf(stderr,"Writing message:\n");
	print_buff((u_int8_t *)msg,msg_size);

	return (msg_size);
}

int read_string(int channel_fd, const char * str)
{
	size_t read_len;
	u_int8_t * msg;

  read_len = read_msg(channel_fd,&msg);
	if(	(read_len != strlen(str) ) ||
   		 memcmp(msg,str,strlen(str)) )
		return (-1);

	return (0);
}


 //len = length of result
void byte2long64(uint8_t* bytes, unsigned int len,  uint64_t* result)
{
  int i;
 for(i=0;i<len; i++)  //init
  {
    result[i] = 0;
  }

  for(i=0;i<len; i++)
  {
    result[i/3] <<=8;
    result[i/3] ^=bytes[i];
  }  
}

//len = length of result
void long642byte(uint64_t* lmsg, unsigned int n, uint8_t* bytes)
{
 uint64_t c=0;
 int i;
 for(i=0; i<n; i++)
 {
  c = 0xFF0000;
  c = lmsg[i/3]&c;

  bytes[i] = c>>16;

  lmsg[i/3]<<=8;
 }
}

void cipher_suite_table(char suite_id, uint8_t *cipher_id,
                        uint8_t *hash, uint8_t* asymm_cipher) {
  switch (suite_id) {
    case 'A':
      *cipher_id = 1;
      *hash = 4;
      *asymm_cipher = 5;
      break;
    case 'B':
      *cipher_id = 2;
      *hash = 4;
      *asymm_cipher = 6;
      break;
    case 'C':
      *cipher_id = 3;
      *hash = 4;
      *asymm_cipher = 6;
      break;
    case 'D':
      *cipher_id = 4;
      *hash = 4;
      *asymm_cipher = 6;
      break;        
  }
}

void decrypt_m(uint8_t cipher_id, uint8_t *c, int c_len, uint8_t* k) {
   int i;
   uint64_t key=0x0;
   uint64_t iv =0x000000;
  if (cipher_id == 1) {  
  Bunnycbc_decrypt(c, c_len, k, iv, c);
  }
  else if (cipher_id == 2 || cipher_id == 3) {
    if (cipher_id == 2) {       
      byte2long64(k, 64, &key);
      all5_decrypt(c, key, c, c_len*8);
    }
    else {
       byte2long64(k, 64, &key);
       maj5_decrypt(c, key, c, c_len*8);
    }
    
  }

}
void enc_msg(uint8_t cipher_id, uint8_t* c, unsigned int length, uint8_t* k)
{
 int i;
 uint64_t iv =0x000000, key=0x0;
 if (cipher_id == 1) {
   BunnyTncbc_encrypt(c, length, k, iv, c);
  }
  else 
    if (cipher_id == 2) {     
      byte2long64(k, 64, &key);
      all5_encrypt(c, key, c, length*8);
      printf("key\n");
    }
    else if (cipher_id == 3){
      byte2long64(k, 64, &key);
      maj5_encrypt(c, key, c, length*8);
    }   
}

int readRsaKey(char* filename, BIGNUM* n, BIGNUM* e)
{
  FILE *fp;
  if ((fp = fopen(filename, "r")) == NULL) {  //"client_folder/server_rsa_public_key.txt"
    fprintf(stderr, "Error while opening RSA key file...\n");
    return -1;
  }
  char rsakey[130];
  fscanf(fp, "%s", rsakey);
  BN_hex2bn(&n,rsakey);
  fscanf(fp, "%s", rsakey);
  BN_hex2bn(&e, rsakey);
  fclose(fp);  
  return 0;
}

int readClientsPublicRsaKey(char* filename, char* clientname, BIGNUM* n, BIGNUM* e)
{
    int done;
    FILE *fp;
    if ((fp = fopen(filename, "r")) == NULL) { //"server_folder/clients_rsa_public_keys.txt"
      fprintf(stderr, "Error while getting clients RSA public keys...\n");
      return -1;
    }
    done = 0;
    uint8_t ntmp[130], etmp[130];
    char client[130];
    while (!feof(fp)) {
      fscanf(fp, "%s %s %s", client, ntmp, etmp);    
      if (strcmp(client, clientname) == 0) {
        BN_hex2bn(&n, (const char *) ntmp);
        BN_hex2bn(&e, (const char *) etmp);
        return 1;
      }
    }
    if (done == 0) {
      fprintf(stderr, "Error: client not recognized\n");
      return -1;
    }
    fclose(fp);  
  return 0;
}
