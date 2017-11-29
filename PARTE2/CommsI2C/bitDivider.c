#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void bitFragment(int data, int label){
	int a = 255; //1111 1111
	int b = 3; //11
	int first8 = a & data;	//guarda 8bits menos significativos da data, fazendo um OR com 1111 1111
	int last8 = 0;			
	int shifted = data >> 8;	//guarda os 2 bits mais sifniciativos da data, empurrando os 8 menos significativos para fora
	
	shifted = shifted & b;		//garantir que são mesmo guardados apenas os 2 bits mais significativos, fazendo um AND com 11
	last8 =  label << 2;		//acrescenta dois zeros ao fim da label para juntar os 2 bits de data
	last8 = last8|shifted;		//junta a label e os dois bits de data com OR
	
	printf("first8 = %d\n",first8);//wire.write (first8);
	printf("last8 = %d\n",last8);//wire.write (last8);
	
}

//first8 é o primeiro byte que recebes, 8bits de data
//last8 é o segundo (e ultimo), 2bits data + 6bits label
void bitAssembler(int first8, int last8) {
	int b = 3; //11
	int labelBits = 0;
	int dataBits = 0;
	
	dataBits = last8 & b;	//do segundo byte guardam-se os bits que fazem parte da data fazendo AND com 11
	dataBits = dataBits << 8;	//abre-se espaço para juntar a restante data, acrescentam-se 8 zeros, para um total de 10bits
	dataBits = dataBits | first8;	//junta-se os 2bits ao 1º byte recebido só com data
	
	labelBits = last8 >> 2;		//os 6bits mais significativos do segundo byte são a label, empurrar os 2bits de data fora para ficar só com a label
	
	printf("assembled data = %d\n", dataBits);
	printf("assembled label = %d\n", labelBits);
}

int main(int argc, char const *argv[]) {
	
	int data = 1000;
	int label = 4;
	
	bitFragment(data, label);	
	
	//imaginando que se recebe pelo wire first8 = 232, last8 = 19 ->(label = 4 e data = 1000)
	bitAssembler(232, 19);
	
	return 1;
}
