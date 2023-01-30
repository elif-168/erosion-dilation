#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include "image_processing.cpp"

using namespace std;
//bu koddaki sadece assembly kýsmý bana aittir

void Dilation(int n, int filter_size, short* resimadres_org);
void Erosion(int n, int filter_size, short* resimadres_org);

int main(void) {
	int M, N, Q, i, j, filter_size;
	bool type;
	int efile;
	char org_resim[100], dil_resim[] = "dilated.pgm", ero_resim[] = "eroded.pgm";
	do {
		printf("Orijinal resmin yolunu (path) giriniz:\n-> ");
		scanf("%s", &org_resim);
		system("CLS");
		efile = readImageHeader(org_resim, N, M, Q, type);
	} while (efile > 1);
	int** resim_org = resimOku(org_resim);

	printf("Orjinal Resim Yolu: \t\t\t%s\n", org_resim);

	short* resimdizi_org = (short*)malloc(N * M * sizeof(short));

	for (i = 0; i < N; i++)
		for (j = 0; j < M; j++)
			resimdizi_org[i * N + j] = (short)resim_org[i][j];

	int menu;
	printf("Yapmak istediginiz islemi giriniz...\n");
	printf("1-) Dilation\n");
	printf("2-) Erosion\n");
	printf("3-) Cikis\n> ");
	scanf("%d", &menu);
	printf("Filtre boyutunu giriniz: ");
	scanf("%d", &filter_size);

	switch (menu) {
	case 1:
		Dilation(N * M, filter_size, resimdizi_org);
		resimYaz(dil_resim, resimdizi_org, N, M, Q);
		break;
	case 2:
		Erosion(N * M, filter_size, resimdizi_org);
		resimYaz(ero_resim, resimdizi_org, N, M, Q);
		break;
	case 3:
		system("EXIT");
		break;
	default:
		system("EXIT");
		break;
	}

	system("PAUSE");
	return 0;
}

void Dilation(int n, int filter_size, short* resim_org) {
        	
	__asm {
		MOV EAX, filter_size
		SHR EAX, 1
		MOV filter_size, EAX

		MOV EDX, 512
		SUB EDX, EAX

		MOV n, 512

		MOV EBX, filter_size

		L1 : CMP EBX, EDX
		     JE EX1

		MOV ESI, filter_size

		L2 : CMP ESI, EDX
		     JE EX2
		
			INC filter_size

			MOV EAX , 0 // max = 0

            XOR ECX, ECX
		    L3 : CMP ECX, filter_size
                 JE EX3

       
            // islem baslangici
			// islem1
				PUSH EAX
				PUSH EDX
					MOV EAX, 512
					MUL EBX 
					ADD EAX, ESI // eax = 512*i + j
					ADD EAX, ECX // eax = 512*i + j + k
					SHL EAX, 1

					MOV EDI, EAX
				POP EDX
				POP EAX
				
				PUSH EBX

				MOV EBX, resim_org

				CMP WORD PTR[EBX + EDI], AX
					JB C1

					MOV AX, WORD PTR[EBX + EDI]
					C1:
				POP EBX
			// islem2

					PUSH EAX
					PUSH EDX

						MOV EAX, 512
						MUL EBX
						ADD EAX, ESI
						SUB EAX, ECX
						SHL EAX, 1

						MOV EDI, EAX
					POP EDX
					POP EAX

					PUSH EBX

						MOV EBX, resim_org

						CMP WORD PTR[EBX + EDI], AX
						JB C2
						MOV AX, WORD PTR[EBX + EDI]
						C2:
					POP EBX
			// islem3
					PUSH EAX
					PUSH EDX

						MOV EAX, EBX
						SUB EAX, ECX
						MUL n
						ADD EAX, ESI // eax = 512*(i-k) + j

						SHL EAX, 1

						MOV EDI, EAX
					POP EDX
					POP EAX

					PUSH EBX

						MOV EBX, resim_org

						CMP WORD PTR[EBX + EDI], AX
						JB C3
						MOV AX, WORD PTR[EBX + EDI]
						C3:
					POP EBX
			// islem4
					PUSH EAX
					PUSH EDX
						MOV EAX, EBX
						ADD EAX, ECX
						MUL n
						ADD EAX, ESI  // eax = 512*(i+k) + j
						SHL EAX, 1

						MOV EDI, EAX
					POP EDX
					POP EAX

					PUSH EBX

					MOV EBX, resim_org

					CMP WORD PTR[EBX + EDI], AX
					JB C4
					MOV AX, WORD PTR[EBX + EDI]
						C4:
					POP EBX

			// islemler bitince elimizde max olmali

					PUSH EAX
					PUSH EDX

						MOV EAX, EBX
						MUL n
						ADD EAX, ESI
						SHL EAX, 1

						MOV EDI, EAX

					POP EDX
					POP EAX

					PUSH EBX

						MOV EBX, resim_org

						CMP WORD PTR[EBX + EDI], AX
						JB C5
						MOV AX, WORD PTR[EBX + EDI]
						C5:

					POP EBX

	        INC ECX
		 	JMP L3
            // loop 3 bitis
			EX3 :
			DEC filter_size

			PUSH AX // max stacke pushlandÄ±

			INC ESI
			JMP L2


			EX2 :
			INC EBX
			JMP L1

				EX1 :

				MOV EBX, 512
				SUB EBX, filter_size

				MOV EDX, filter_size


				LOOP1 : CMP EBX, EDX
						JE EXIT1
								
						MOV ESI, 512
						SUB ESI, filter_size
						LOOP2 : CMP ESI, EDX
						JE EXIT2
						push edx
							mov eax , ebx
							mov ecx , 512
							mul ecx // eax = 512 * i
							add eax , esi
							shl eax , 1
							mov edi , eax
						pop edx
							POP AX
						push ebx
							MOV EBX, resim_org
							MOV WORD PTR[EBX + EDI], AX
						pop ebx

						DEC ESI
						JMP LOOP2

						EXIT2:
							DEC EBX
							JMP LOOP1

					EXIT1:

			
	}

	printf("\nDilation islemi sonucunda resim \"dilated.pgm\" ismiyle olusturuldu...\n");
}

void Erosion(int n, int filter_size, short* resim_org) {

	__asm {
		//KODUNUZU BURAYA YAZINIZ, ASAGIDAKI 5 ORNEK ERISIM ICIN VERILMISTIR SIZ YAZMAYA BASLAMADAN SILINIZ
		MOV EAX, filter_size
		SHR EAX, 1
		MOV filter_size, EAX

		MOV EDX, 512
		SUB EDX, EAX

		MOV n, 512

		MOV EBX, filter_size

		L1 : CMP EBX, EDX
		JE EX1

		MOV ESI, filter_size

		L2 : CMP ESI, EDX
		JE EX2

		INC filter_size

		MOV EAX, 255 // min = 255

		XOR ECX, ECX
		L3 : CMP ECX, filter_size
		JE EX3


		// islem baslangici
		// islem1
		PUSH EAX
		PUSH EDX
		MOV EAX, 512
		MUL EBX
		ADD EAX, ESI // eax = 512*i + j
		ADD EAX, ECX // eax = 512*i + j + k
		SHL EAX, 1

		MOV EDI, EAX
		POP EDX
		POP EAX

		PUSH EBX

		MOV EBX, resim_org

		CMP WORD PTR[EBX + EDI], AX
		JA C1

		MOV AX, WORD PTR[EBX + EDI]
		C1:
		POP EBX
			// islem2

			PUSH EAX
			PUSH EDX

			MOV EAX, 512
			MUL EBX
			ADD EAX, ESI
			SUB EAX, ECX
			SHL EAX, 1

			MOV EDI, EAX
			POP EDX
			POP EAX

			PUSH EBX

			MOV EBX, resim_org

			CMP WORD PTR[EBX + EDI], AX
			JA C2
			MOV AX, WORD PTR[EBX + EDI]
			C2:
		POP EBX
			// islem3
			PUSH EAX
			PUSH EDX

			MOV EAX, EBX
			SUB EAX, ECX
			MUL n
			ADD EAX, ESI // eax = 512*(i-k) + j

			SHL EAX, 1

			MOV EDI, EAX
			POP EDX
			POP EAX

			PUSH EBX

			MOV EBX, resim_org

			CMP WORD PTR[EBX + EDI], AX
			JA C3
			MOV AX, WORD PTR[EBX + EDI]
			C3:
		POP EBX
			// islem4
			PUSH EAX
			PUSH EDX
			MOV EAX, EBX
			ADD EAX, ECX
			MUL n
			ADD EAX, ESI  // eax = 512*(i+k) + j
			SHL EAX, 1

			MOV EDI, EAX
			POP EDX
			POP EAX

			PUSH EBX

			MOV EBX, resim_org

			CMP WORD PTR[EBX + EDI], AX
			JA C4
			MOV AX, WORD PTR[EBX + EDI]
			C4:
		POP EBX

			// islemler bitince elimizde max olmali

			PUSH EAX
			PUSH EDX

			MOV EAX, EBX
			MUL n
			ADD EAX, ESI
			SHL EAX, 1

			MOV EDI, EAX

			POP EDX
			POP EAX

			PUSH EBX

			MOV EBX, resim_org

			CMP WORD PTR[EBX + EDI], AX
			JA C5
			MOV AX, WORD PTR[EBX + EDI]
			C5:

		POP EBX

			INC ECX
			JMP L3
			// loop 3 bitis
			EX3 :
		DEC filter_size

			PUSH AX // max stacke pushlandÄ±

			INC ESI
			JMP L2


			EX2 :
		INC EBX
			JMP L1

			EX1 :

		MOV EBX, 512
			SUB EBX, filter_size

			MOV EDX, filter_size


			LOOP1 : CMP EBX, EDX
			JE EXIT1

			MOV ESI, 512
			SUB ESI, filter_size

			LOOP2 : CMP ESI, EDX
			JE EXIT2

			push edx

			mov eax, ebx
			mov ecx, 512
			mul ecx // eax = 512 * i
			add eax, esi
			shl eax, 1
			mov edi, eax

			pop edx

			POP AX

			push ebx

			MOV EBX, resim_org
			MOV WORD PTR[EBX + EDI], AX
			pop ebx

			DEC ESI
			JMP LOOP2


			EXIT2 :
		DEC EBX
			JMP LOOP1

			EXIT1 :

	SON1:
	}


	printf("\nErosion islemi sonucunda resim \"eroded.pgm\" ismiyle olusturuldu...\n");
}
