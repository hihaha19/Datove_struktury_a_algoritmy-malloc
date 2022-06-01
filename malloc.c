#include <stdio.h>
#include <string.h>

#pragma pack(1)
typedef struct blok {
    char volny;                     //1 b
    struct blok *dalsi;             //4
    int velkost;                    //4
}BLOK;

unsigned char* pamat;

void memory_init(void *ptr, unsigned int size) {
    pamat = (unsigned char*)ptr;

    BLOK hlavicka_volneho_bloku;                                            
    *((BLOK**) ptr) = (BLOK*)((char*)(ptr) + sizeof(BLOK*));              
    hlavicka_volneho_bloku.volny = 1;                                      
    hlavicka_volneho_bloku.dalsi = NULL;                                    
    hlavicka_volneho_bloku.velkost = size - sizeof(BLOK*) - sizeof(BLOK);   
    *((BLOK*)((char*)(ptr)+sizeof(BLOK*))) = hlavicka_volneho_bloku;      
}

void* memory_alloc(unsigned int size) {
    void* result;
    BLOK *hlavicka_bloku = NULL, *novy_blok = NULL, *najvhodnejsi = NULL;
    int min_rozdiel = -1, rozdiel = -1;
   
    hlavicka_bloku = (*(BLOK**)pamat);  //v hlavicke bloku je prvy blok zoznamu
    
    //ak chcem alokovat nulovu velkost
    if (size == 0) return NULL;

    //ak v hlavicke bloku nic nemam
    if (hlavicka_bloku == NULL) {  
        return NULL;
    }

    int y = 0;

    //hlada volnu hlavicku s potrebnou velkostou
    while (hlavicka_bloku != NULL) {
        if (y == 0 && hlavicka_bloku->volny == 1 && hlavicka_bloku->velkost >= size) { 
            min_rozdiel = (hlavicka_bloku->velkost - size);    
            y++;    
        }
        if (hlavicka_bloku->volny == 1 && hlavicka_bloku->velkost >= size) {       
            rozdiel = (hlavicka_bloku->velkost - size); 
            if (min_rozdiel >= rozdiel) {   
                min_rozdiel = rozdiel;                         
                najvhodnejsi = hlavicka_bloku;              
            }
        }
        hlavicka_bloku = hlavicka_bloku->dalsi;
    }

    //ak velkost bloku je mensia ako potrebna velkost
    if (najvhodnejsi == NULL) {
        return NULL;
    }

    //vytvorim novy blok, ktory dam ako ->dalsi posledneho obsadeneho bloku
      //upravim volnu hlavicku na obsadenu
    if (najvhodnejsi->velkost >= (size + sizeof(BLOK))) {
         novy_blok = (BLOK*)(((char*)najvhodnejsi) + sizeof(BLOK) + size);
         if (najvhodnejsi->dalsi == NULL)   novy_blok->dalsi = NULL;
         else novy_blok->dalsi = najvhodnejsi->dalsi;
         
         novy_blok->volny = 1;
         novy_blok->velkost = (najvhodnejsi->velkost - sizeof(BLOK) - size);
         najvhodnejsi->volny = 0;
         najvhodnejsi->velkost = size;
         najvhodnejsi->dalsi = novy_blok;
    }
    
   //ak najvhodnejsi nema dostatok miesta na vytvorenie novej hlavicky
    else   najvhodnejsi->volny = 0;
    
    //vratim pointer na novo obsadeny blok
    result = (void*)(((char*)najvhodnejsi) + sizeof(BLOK));
}



int memory_free(void* valid_ptr) {
    BLOK *uvolnujuci_blok = NULL, *predchadzajuci_blok = NULL, *nasledujuci_blok = NULL;
    uvolnujuci_blok = (*(BLOK**)pamat);  

    while (uvolnujuci_blok + 1 != valid_ptr) 
    {
        nasledujuci_blok = uvolnujuci_blok->dalsi->dalsi;
        predchadzajuci_blok = uvolnujuci_blok;
        uvolnujuci_blok = uvolnujuci_blok->dalsi;
    }

    if (uvolnujuci_blok->volny == 0)   uvolnujuci_blok->volny = 1;
    else return 1;
   
    //ak predchadzajuci aj nasledujuci blok existuju a su volne
    if (predchadzajuci_blok != NULL && nasledujuci_blok != NULL && predchadzajuci_blok->volny == 1 && nasledujuci_blok->volny == 1) {
        predchadzajuci_blok->velkost = (predchadzajuci_blok->velkost + uvolnujuci_blok->velkost + nasledujuci_blok->velkost + 
            2*sizeof(BLOK));
        predchadzajuci_blok->dalsi = nasledujuci_blok->dalsi;
    }

    //ak je volny iba predchadzajuci blok, spoji ich
    else if (predchadzajuci_blok != NULL && predchadzajuci_blok->volny == 1) {
           predchadzajuci_blok->velkost = (predchadzajuci_blok->velkost + uvolnujuci_blok->velkost + sizeof(BLOK));
           predchadzajuci_blok->dalsi = uvolnujuci_blok->dalsi;  
    }
   
    //ak je volny iba nasledujuci blok, spoji ich
    else if (nasledujuci_blok != NULL && nasledujuci_blok->volny == 1) {
             uvolnujuci_blok->velkost = (nasledujuci_blok->velkost + uvolnujuci_blok->velkost+sizeof(BLOK));
             uvolnujuci_blok->dalsi = nasledujuci_blok->dalsi;
    }
    return 0;
}


int memory_check(void* ptr) {
    BLOK* hlavicka_bloku;
    hlavicka_bloku = (*(BLOK**)pamat);

    while (hlavicka_bloku != NULL) {
        if ((hlavicka_bloku + 1) == ptr && hlavicka_bloku->volny == 0)  return 1;
        else hlavicka_bloku = hlavicka_bloku->dalsi;
    }
    return 0;
}

//testuje velkost spravcu pamate 200 s blokmi velkosti 8 bytov
void test_jedna() {
    char region[200];
    memory_init(region, 200);
    char* prvy = (char*)memory_alloc(8);
    char* druhy = (char*)memory_alloc(8);
    char* treti = (char*)memory_alloc(8);
    char* stvrty = (char*)memory_alloc(8);
    char* piaty = (char*)memory_alloc(8);
    char* siesty = (char*)memory_alloc(8);
    char* siedmy = (char*)memory_alloc(8);
    char* osmy = (char*)memory_alloc(8);
    char* deviaty = (char*)memory_alloc(8);
    char* jedenasty = (char*)memory_alloc(8);
    char* dvanasty = (char*)memory_alloc(8);

    int pocet_alokovanych_blokov = 0, velkost = 0;
    BLOK* hlavicka_bloku;
    hlavicka_bloku = (*(BLOK**)pamat);
    while (hlavicka_bloku != NULL && hlavicka_bloku->volny == 0) {
        pocet_alokovanych_blokov++;
        velkost = velkost + hlavicka_bloku->velkost;
        hlavicka_bloku = hlavicka_bloku->dalsi;
    }
    printf("Pocet alokovanych blokov je %d, efektivne vyuzitie pamate je %d\n", pocet_alokovanych_blokov, velkost);
}

void test_dva() {
    char region[100];
    memory_init(region, 100);
    char* prvy = (char*)memory_alloc(10);
    char* druhy = (char*)memory_alloc(10);
    char* treti = (char*)memory_alloc(10);
    char* stvrty = (char*)memory_alloc(10);
    char* piaty = (char*)memory_alloc(10);
    char* siesty = (char*)memory_alloc(10);
    char* siedmy = (char*)memory_alloc(10);
    char* osmy = (char*)memory_alloc(10);
    char* deviaty = (char*)memory_alloc(10);
    char* desiaty = (char*)memory_alloc(10);

    int pocet_alokovanych_blokov = 0;
    BLOK* hlavicka_bloku;
    hlavicka_bloku = (*(BLOK**)pamat);
    while (hlavicka_bloku != NULL && hlavicka_bloku->volny == 0) {
        pocet_alokovanych_blokov++;
        hlavicka_bloku = hlavicka_bloku->dalsi;
    }
    printf("Pocet alokovanych blokov je %d\n", pocet_alokovanych_blokov);
}

//testuje spravcu pamate s velkostou 200 a bloky roznej velkosti od 8 do 24 bytov
void test_tri() {
    char region[200];
    memory_init(region, 200);
    char* prvy = (char*)memory_alloc(18);
    char* druhy = (char*)memory_alloc(10);
    char* treti = (char*)memory_alloc(8);
    char* stvrty = (char*)memory_alloc(24);
    char* piaty = (char*)memory_alloc(24);
    char* siesty = (char*)memory_alloc(20);
    char* siedmy = (char*)memory_alloc(16);
    char* osmy = (char*)memory_alloc(19);
    char* deviaty = (char*)memory_alloc(17);
    char* desiaty = (char*)memory_alloc(18);
    char* jedenasty = (char*)memory_alloc(8);
    char* dvanasty = (char*)memory_alloc(18);

    int pocet_alokovanych_blokov = 0, velkost = 0;
    BLOK* hlavicka_bloku;
    hlavicka_bloku = (*(BLOK**)pamat);
    while (hlavicka_bloku != NULL && hlavicka_bloku->volny == 0) {
        pocet_alokovanych_blokov++;
        velkost = velkost + hlavicka_bloku->velkost;
        hlavicka_bloku = hlavicka_bloku->dalsi;
    }
    printf("Pocet alokovanych blokov je %d, efektivne vyuzitie pamate je %d\n", pocet_alokovanych_blokov, velkost);
}

//spravca pamate ma velkost 10 000 a bloky velkej velkosti
void test_styri() {
    char region[10000];
    memory_init(region, 10000);
    char* prvy = (char*)memory_alloc(1000);
    char* druhy = (char*)memory_alloc(1800);
    char* treti = (char*)memory_alloc(500);
    char* stvrty = (char*)memory_alloc(1587);
    char* piaty = (char*)memory_alloc(2548);
    char* siesty = (char*)memory_alloc(1658);
    char* siedmy = (char*)memory_alloc(907);
  
    int pocet_alokovanych_blokov = 0, velkost = 0;
    BLOK* hlavicka_bloku;
    hlavicka_bloku = (*(BLOK**)pamat);
    while (hlavicka_bloku != NULL && hlavicka_bloku->volny == 0) {
        pocet_alokovanych_blokov++;
        velkost = velkost + hlavicka_bloku->velkost;
        hlavicka_bloku = hlavicka_bloku->dalsi;
    }
    printf("Pocet alokovanych blokov je %d, efektivne vyuzitie pamate je %d\n", pocet_alokovanych_blokov, velkost);
}

//spravca pamate je 100 000 a bloky roznej velkosti
void test_pat() {
    char region[100000];
    memory_init(region, 100000);
    char* prvy = (char*)memory_alloc(8);
    char* druhy = (char*)memory_alloc(16);
    char* treti = (char*)memory_alloc(50000);
    char* stvrty = (char*)memory_alloc(4000);
    char* piaty = (char*)memory_alloc(2548);
    char* siesty = (char*)memory_alloc(1658);
    char* siedmy = (char*)memory_alloc(24);
    char* osmy = (char*)memory_alloc(17);
    char* deviaty = (char*)memory_alloc(59);
    char* desiaty = (char*)memory_alloc(57);
    char* jedenasty = (char*)memory_alloc(10000);
    char* dvanasty = (char*)memory_alloc(25);
    char* trinasty = (char*)memory_alloc(907);
    char* strnasty = (char*)memory_alloc(18000);
    char* patnasty = (char*)memory_alloc(12000);
    char* sestnasty = (char*)memory_alloc(600);
    char* sedemnasty = (char*)memory_alloc(81);

    int pocet_alokovanych_blokov = 0, velkost = 0;
    BLOK* hlavicka_bloku;
    hlavicka_bloku = (*(BLOK**)pamat);
    while (hlavicka_bloku != NULL && hlavicka_bloku->volny == 0) {
        pocet_alokovanych_blokov++;
        velkost = velkost + hlavicka_bloku->velkost;
        hlavicka_bloku = hlavicka_bloku->dalsi;
    }
    printf("Pocet alokovanych blokov je %d, efektivne vyuzitie pamate je %d\n", pocet_alokovanych_blokov, velkost);
}

//spravca pamate ma velkost 48 000 a bloky roznej velkosti
void test_sest() {
    char region[48000];
    memory_init(region, 48000);
    char* prvy = (char*)memory_alloc(23);
    char* druhy = (char*)memory_alloc(3172);
    char* treti = (char*)memory_alloc(4963);
    char* stvrty = (char*)memory_alloc(16);
    char* piaty = (char*)memory_alloc(45);
    char* siesty = (char*)memory_alloc(149);
    char* siedmy = (char*)memory_alloc(24);
    char* osmy = (char*)memory_alloc(17);
    char* deviaty = (char*)memory_alloc(59);
    char* desiaty = (char*)memory_alloc(57);
    char* jedenasty = (char*)memory_alloc(10000);
    char* dvanasty = (char*)memory_alloc(1900);
    char* trinasty = (char*)memory_alloc(199);
    char* strnasty = (char*)memory_alloc(195);
    char* patnasty = (char*)memory_alloc(12000);
    char* sestnasty = (char*)memory_alloc(600);
    char* sedemnasty = (char*)memory_alloc(55);
    char* osemnasty = (char*)memory_alloc(600);
    char* devatnasty = (char*)memory_alloc(600);
    char* dvadsiaty = (char*)memory_alloc(600);
    char* dvadsatyprvy = (char*)memory_alloc(6000);
    char* dvadsiatydruhy = (char*)memory_alloc(19);
    char* dvadsiatytreti = (char*)memory_alloc(879);
    char* dvadsiatystvrty = (char*)memory_alloc(4567);
    char* dvadsiatypiaty = (char*)memory_alloc(1000);
    char* dvadsiatysiesty = (char*)memory_alloc(261);

    int pocet_alokovanych_blokov = 0, velkost = 0;
    BLOK* hlavicka_bloku;
    hlavicka_bloku = (*(BLOK**)pamat);
    while (hlavicka_bloku != NULL && hlavicka_bloku->volny == 0) {
        pocet_alokovanych_blokov++;
        velkost = velkost + hlavicka_bloku->velkost;
        hlavicka_bloku = hlavicka_bloku->dalsi;
    }
    printf("Pocet alokovanych blokov je %d, efektivne vyuzitie pamate je %d\n", pocet_alokovanych_blokov, velkost);
}

void test_sedem() {
    char region[120];
    memory_init(region, 120);

    char* prvy = (char*)memory_alloc(10);
    char* druhy = (char*)memory_alloc(20);
    int a;
    printf("A je %d\n", a = memory_free(druhy));
    printf("A je %d\n", a = memory_free(druhy));
    printf("A je %d\n", a = memory_check(druhy));
}

void test_krajnych_pripadov() {
    char region[100];
    memory_init(region, 100);

    char* nulty = (char*)memory_alloc(100); //pokus o alokaciu viac pamate ako je dostupna, aj ked je cela pamat este volna
    printf("Smernik nulteho je %p\n", nulty);

    char* prvy = (char*)memory_alloc(1);    //pokus o alokaciu najmensej moznej velkosti
    if (prvy) printf("Velkost 1 uspesne alokovana\n");

    char* druhy = (char*)memory_alloc(0);   //pokus o alokaciu velkosti 0
    printf("Alokacia velkosti 0 ma smernik %p\n", druhy);

    char* treti = (char*)memory_alloc(77);  //uplne zaplni volnu pamat

    char* stvrty = (char*)memory_alloc(10); //pokus o alokaciu 10 bytov v pripade, ze v pamati nezostalo ziadne volne miesto
    if (stvrty) printf("Uspesna alokacia");
    else printf("Pamat je plna\n");
   
    memory_free(prvy);
    memory_free(treti);
}


//testovanie funkcie memory free na uvolnenie bloku, ktory sa nachadza medzi dvomi volnymi blokmi, vsetky 3 bloky spoji do jedneho
void test_memory_free() {
    char region[120];
    memory_init(region, 120);
    char* prvy = (char*)memory_alloc(10);
    char* druhy = (char*)memory_alloc(8);
    char* treti = (char*)memory_alloc(16);
    char* stvrty = (char*)memory_alloc(16);
    char* piaty = (char*)memory_alloc(21);

    memory_free(druhy);
    memory_free(stvrty);
    memory_free(treti);

    BLOK* hlavicka_bloku;
    hlavicka_bloku = (*(BLOK**)pamat);
    while (hlavicka_bloku != NULL) {
        printf("Velkost bloku je %d a jeho volnost je %d\n", hlavicka_bloku->velkost, hlavicka_bloku->volny);
        hlavicka_bloku = hlavicka_bloku->dalsi;
    }

    memory_free(prvy);
    memory_free(piaty);
}


void test_memory_check() {
    char region[120];
    memory_init(region, 120);

    char* prvy = (char*)memory_alloc(10);
    char* druhy = (char*)memory_alloc(20);
   
    int a;
    printf("Kontrola prveho smernika da vysledok %d\n", a = memory_check(prvy));
    printf("Kontrola druheho smernika da vysledok %d\n", a = memory_check(druhy));
    printf("Prvy smernik zvacseny o 1 da vysledok %d\n", a = memory_check(prvy+1));
    printf("Druhy smernik zmenseni o 1 da vysledok %d\n", a = memory_check(druhy-1));
}


int main()
{
  char region[200];
  memory_init(region, 200);

    return 0;
}