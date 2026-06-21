//!!! required ddraw.lib in link options/object modules


bool _fastcall initgraph(HWND mwin,int sizex,int sizey,int bpp,int numbbuf,int numimages,int numbitmap);
  // Инициализирует DirectDraw
  // mwin -Указатель на окно , в котором все выводится (тип hWnd)
  // sizex,sizey - размеры экрана (например sizex=800,sizey=600)
  // bpp - количество бит на пиксел (например 16 [65536 цветов])
  // numbbuf - количество видеостраниц (0-только физ. экран)
  // numimages - количество offscreen surfases (0- отсутствуют)
  // numbitmap - bitmap couunt

HDC valloc(int vnum);
  // Определяет , с какой видеостраницей ведется работа
  // vnum - ее номер (0 - физический экран)
  // пред использованием функйий вывода линии , точки , прямоугольника, текста
  // необходимо вызывать VAlloc , а потом освобождать видеостраницу  (VFree)
  // возвращает контекст устройства

void vfree();
  // Освобождает занятую функцией VAlloc видеостраницу


bool flip (int numpage);
  // Меняет местами экран и numpage видеостраницу

void setbkcolor(int mcolor);
  // Выставляет цвет фона mcolor

void cleardevice();
  // Очищает экран цветом Background

void setcolor(int mcolor);
  // устанавливает цвет для текста и графических примитивов

void setlinewidth(int width);
 //устанавливает толщину линий

bool setfontindirect(LOGFONT *lf);
	//set font for textout
	//lf - Pointer to a LOGFONT structure that defines the characteristics of the logical font. 


bool setfont(char *fontname="Times New Roman",DWORD charset=DEFAULT_CHARSET,
			 int height=0,int width=0,int orientation=0,int weight=0,bool italic=false,
			 bool underline=false,bool strikeout=false);
	//set font for textout
	//fontname - name of the font
	//charset - type of charset:DEFAULT_CHARSET,ANSI_CHARSET,HEBREW_CHARSET,RUSSIAN_CHARSET,OEM_CHARSET...
	//height- height of the font
	//width - width of the font
	//orientation - Specifies the angle, in tenths of degrees, between each character's base line and the x-axis of the device. 
	//weight - Specifies the weight of the font in the range 0 through 1000. For example, 400 is normal and 700 is bold. If this value is zero, a default weight is used. 
	//			The following values are defined for convenience: Value Weight 
	//			FW_DONTCARE 0 
	//			FW_THIN 100 
	//			FW_EXTRALIGHT 200 
	//			FW_ULTRALIGHT 200 
	//			FW_LIGHT 300 
	//			FW_NORMAL 400 
	//			FW_REGULAR 400 
	//			FW_MEDIUM 500 
	//			FW_SEMIBOLD 600 
	//			FW_DEMIBOLD 600 
	//			FW_BOLD 700 
	//			FW_EXTRABOLD 800 
	//			FW_ULTRABOLD 800 
	//			FW_HEAVY 900 
	//			FW_BLACK 900 
	//italic - Specifies an italic font if set to TRUE
	//underline - Specifies an underlined font if set to TRUE. 
	//strikeout - Specifies a strikeout font if set to TRUE. 




void textout (int mx,int my,char *mstr);
  // выводит текст mstr с позиции mx,my в окне

void moveto(int mx,int my);
  // перемещает экранный курсор в позицию mx,my на экране

void lineto (int mx,int my);
  // строит линию от экранного курсора к позиции mx,my

void line (int mx,int my,int mx1,int my1);
  //  строит линию (mx,my) - (mx1,my1)

void rectangle(int mx,int my,int mx1,int my1);
  //  рисует прямоугольник

void setfillcolor (int mcol);
  // устанавливает цвет закраски  

void bar (int mx,int my,int mx1,int my1);
  // рисует закрашенный прямоугольник

void fillcircle (int sx,int sy,int rad);
  // рисует закрашенную окружность
  
void fillellipse (int sx,int sy,int rad,int rae);
  // рисует закрашенный еллипс

void circle (int sx,int sy,int rae);
  // рисует окружность

void ellipse (int sx,int sy,int rae,int ral);
  // рисует эллипс

void putpixel(int sx,int sy,int colr);
  // рисует точку

int getpixel(int sx,int sy);
  // возвращает цвет точки

bool loadbmp (int mx,int my,char *mpath);
  // выводит BMP файл (mpath) на экран с позиции mx,my

int loadbmptoptr(char *mpath);
  // выводит BMP файл (mpath) в offscreen surfase

bool putsprite (int mx,int my,int bnum,int mpage);
  // выводит изображение на экран. Не выводит 0 точки.

bool putimage (int mx,int my,int bnum,int mpage);
  // выводит изображение на экран

int getimage (int mx,int my,int mx1,int my1,int mpage);
  // создает offscreen surface и вырезает туда кусок
  // изображения c экрана или видеостраницы

bool virtputimage(int mx,int my,int bnum,int bnum1);
  // выводит изображение на offscreen surfase

bool virtputsprite(int mx,int my,int bnum,int bnum1);
  // выводит изображение на offscreen surfase. Не выводит
                      //  0 точки.

int virtgetimage (int mx,int my,int mx1,int my1,int mpage);
  // создает offscreen surface и вырезает туда кусок
  // изображения c offscreen surfase

bool putimagetoscreen (int mx,int my,int mx1,int my1,int mx3,int my3,int mnum,int mpage);
  // копирует на экран кусок offscreen surfase
  // (mx,my)-(mx1,my1)-координаты выводимого участка
  // mx3,my3 - координаты на экране куда выводится
  // mnum - указатель на offscreen surfase
  // mpage- номер видеостраницы . 0- экран

bool putspritetoscreen (int mx,int my,int mx1,int my1,int mx3,int my3,int mnum,int mpage);
  // копирует на экран кусок offscreen surfase. 
  // аналог putimagetoscreen но не выводит точки с 0 цветом.

bool putimagetoptr (int mx,int my,int mx1,int my1,int mx3,int my3,int mnum,int mnum1);
  // аналог putimagetoscreen но выводит не на экран а в offscreen surfase

bool putspritetoptr (int mx,int my,int mx1,int my1,int mx3,int my3,int mnum,int mnum1);
 // аналог putspritetoscreen но выводит не на экран а в offscreen surfase

void prelease();
 //удаляет последний указатель на картинку

void release(int n);
//удаляет указатель n на картинку

void closegraph();
  // "Закрытие" графики

void restoreimage(int num);
 //restore lost image (afther ALT+TAB)

void restorescreen();
//restore screen

bool reloadbmptoptr(int bnum,char *mpath);
//reload bmp image to activated image bnum

int fillbitmap(int numi);
//create bitmap and load image to bitmap from offscreen surface
//numi - nuber of compatible image

bool freebitmap();
//put image from bitmap to offscreen surface

bool updatebitmap(int mx,int my,int width1,int height1,int bnum,int bmnum);
//

bool removebitmap();
//

bool putbitmap(int nbm);