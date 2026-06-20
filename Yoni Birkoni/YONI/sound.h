//!!! required dsound.lib (dxguid.lib)??? in link options/object modules



unsigned long int const DS_NORMAL=1,
                        DS_PRIORITY=2,
                        DS_EXCLUSIVE=3;

bool initsound (HWND hwnd,unsigned long int daccess,unsigned long numbuf);

void closesound();

unsigned long int loadwav(char *mpath);

unsigned long int loadwavparth(char *mpath,unsigned long int filesize);

unsigned long int getbufsize(unsigned long int nbuf);

bool playbuf(unsigned long int nbuf,bool mloop);

unsigned long int getbufpos(unsigned long int nbuf);

bool setbufpos(unsigned long int nbuf,unsigned long int npos);

bool stopbuf(unsigned long int nbuf);

bool bufplaying (unsigned long int nbuf);

long int getbufvol (unsigned long int nbuf);

bool setbufvol (unsigned long int nbuf,long int yy);

unsigned long int getbuffreq (unsigned long int nbuf);

bool setbuffreq (unsigned long int nbuf,unsigned long int yy);

long int getbufpan (unsigned long int nbuf);

bool setbufpan (unsigned long int nbuf,long int yy);

void pdestroybuf();