// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <math.h> 
#include <stdint.h>
#include <time.h>
#define DATA_LEN 1024

#define PORT 8080 

struct msj{
	
	int8_t numSeq;
	int CRC8;	
	char data[DATA_LEN];
	int16_t lenght;
	int8_t tipo;
};
typedef struct msj MSJ;



int getSocket(){// Creating socket file descriptor 
	int server_fd;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    return server_fd;
}

int configSocket(int server_fd, int opt){// Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR |SO_REUSEPORT, &opt, sizeof(opt)) ) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    return opt;
}

struct sockaddr_in configAddress()
{
	struct sockaddr_in address; 
	address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
    return address;
}

void setPort(int server_fd, struct sockaddr_in address){// Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
}

void setListen(int server_fd)
{
	if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }
}

float obtenConfirmacion(){
	return  ((rand()%10000)/100.0f);//maximo 99.99
}

float obtenTemporizador(){
	return  ((rand()%6000)/100.0f);//maximo 60.0 seg
}

int obtenValor(char* errorOTiempo, char *porError, char* porTem){

	if(strcmp(errorOTiempo,"-e")==0){
		return atof(porError);
	}else if(strcmp(errorOTiempo,"-p")==0){
		return atof(porTem);
	}else{
		return 0.0;
	}

}



int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket, valread;
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address);
    unsigned long long numPaquetes;
    char buffer[1024]={0};
    char newBuffer[1024]={0}; 
    char direccion[100]={0}; 
    char bandera[1];
    char banderaFin[1] = "";
    float errorMax;
    float  tempMax;
    char* error=strdup(argv[1]);
    char* temp = strdup(argv[2]);
    char* porError = strdup(argv[3]);//obtencion de los porcentajes de error y tiempo
    char* porTemp = strdup(argv[4]);
    float nuevoError;
    float nuevoTemp;
    
    
  	FILE *archivo;
  	srand(time( NULL ));//generar semilla
	server_fd = getSocket();
	opt = configSocket(server_fd, opt);
	address = configAddress();
	setPort(server_fd, address);
	setListen(server_fd);
    	
    	errorMax = obtenValor(error,porError,porTemp);
    	tempMax = obtenValor(temp,porError,porTemp);
	
	
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen))<0) { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 
    read( new_socket , direccion, 100);//leemos el nombre del archivo que nos llega del servidor
    archivo = fopen (direccion, "rb");//abrimos el archivo para lectura
 
	if (archivo == NULL){
        printf("El archivo No Existe.\n");
        bandera[0] = '0';
        send( new_socket, bandera, 1, 0);//enviamos la bandera = '0', si no existe al cliente
        return 0;
    }
	else{
		
		bandera[0] = '1';
		send( new_socket, bandera, 1, 0);//enviar bandera = '1', si existe al cliente

		
		fseek(archivo, 0, SEEK_SET);//nos posicionamos al principio del archivo

		
		while((valread= fread (newBuffer, 1, 1024, archivo)) != EOF && !(valread == 0)){//mientras existan datos el servidor  estara enviando paquetes
			nuevoError = obtenConfirmacion();
			nuevoTemp = obtenTemporizador();
			
			if(nuevoError<=errorMax){
			
			}
			if(nuevoTemp <= tempMax){
			
			}
			
			send(new_socket, newBuffer, valread, 0 ); //enviando el numero de  datos de valread
		}
		
		send(new_socket, banderaFin, 1, 0 );//Bandera para enviar el fin de archivos, el cual marca el fin del envio y avisamos al cliente
		
		printf("Archivo %s se ha enviado correctamente \n",direccion);
	    	
	    	fclose (archivo);
	    	
	} 
    return 0; 
} 
