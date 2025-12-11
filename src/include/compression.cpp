#include "coding_factory.h"

#define MAXDOC 26000000



int prepare(int type)
{
	char fns[256];
	char word[64];
	int np = 0;
	int ndoc = 100;
	unsigned* doc = new unsigned[MAXDOC];
	unsigned* test = new unsigned[MAXDOC];
	
	unsigned* docr = new unsigned[MAXDOC];
	
	unsigned* redoc = new unsigned[MAXDOC];

	//input pointer
	unsigned* doc_ptr;

	//output pointer
	unsigned* docr_ptr;

	unsigned 
			int id, occ;
	FILE* fdw = fopen("/home/leo/qwords.txt", "wb");

	coding_factory cf;
	coding* coder = cf.get_coder(type);
       

	int block_size = 128;
	int offset = 0;
	int i;
	int flag = 0;

//	while (!feof(fdw))
//	{
		flag  = 0;
//		memset(word, 0, 64);
//		fscanf(fdw, "%s\t%u\t%u\n", word, &id, &occ);
//		ndoc = get_data(doc, word, &np);
		printf("\n\n\n"); fflush(NULL);
		for (int cntr = 0; cntr < ndoc; cntr++) {
			doc[cntr] = rand() % ndoc + 1;
			printf("%d: Input: %d\n", cntr, doc[cntr]);
			fflush(NULL);
		}

		doc_ptr = doc;
		docr_ptr = docr;

		printf("%d", ndoc); fflush(NULL);
		if ( ndoc > 0)
		{
			int x = 0 , block = 0;
			for ( i = 0; i < ndoc; i+= block_size ){

				offset = coder->Compression(doc_ptr, docr_ptr, block_size);
				//fwrite(&docr, sizeof(unsigned int), block_size, fdw);

				for (x = block * block_size; x < (block+1) * block_size; x++) {
					fwrite(&docr[x], sizeof(unsigned), 1, fdw);
					printf("c: number %d: Compressed: %u, Original:%u\n", 
						x, docr[x], doc[x]);
					fflush(NULL);
				}
				block++;

				doc_ptr += block_size;
				docr_ptr += offset;
			}

			printf("compression finished.\nStarting decompression\n"); fflush(NULL);
			doc_ptr = redoc;
			docr_ptr = docr;

			block = 0;
			for ( i = 0; i < ndoc; i+= block_size){

				offset = coder->Decompression(docr_ptr, doc_ptr, block_size);

				docr_ptr += offset;
				doc_ptr += block_size;

				for (x = block * block_size; x < (block+1) * block_size; x++) {
//					printf("d: number %d: Compressed: %u, Original:%u\n", x, docr[x], redoc[x]);
//					fflush(NULL);
				}
				block++;
				//getchar();

			}

			for ( i = 0; i < ndoc; i++)
			{
				if ( doc[i] != redoc[i])
					flag = 1;
			}

			if ( flag == 1)
				cout << "the result is not right!" << endl;
			else
				cout << "Yeah,!"<<endl;
		}
//	}

	fclose (fdw);
	
	
	FILE* fdd = fopen("/home/leo/qwords.txt", "rb");

	for (int k = 0; k < ndoc; k++) {
		fread(&test[k], sizeof(unsigned), 1 ,fdd);
		printf("%d:%u\n", k, test[k]);
		fflush(NULL);
	}
	
	fclose(fdd);
	delete[] doc;
	delete[] docr;
	delete[] redoc;

	
}

int main()
{
	prepare(1);
}
