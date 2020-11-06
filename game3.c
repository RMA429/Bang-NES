
#include <stdlib.h>
#include <string.h>

#include <stdlib.h>
#include <string.h>

// include NESLIB header
#include "neslib.h"

// include CC65 NES Header (PPU)
#include <nes.h>

// APU (sound) support
#include "apu.h"
//#link "apu.c"

// link the pattern table into CHR ROM
//#link "chr_generic.s"

// BCD arithmetic support
#include "bcd.h"
//#link "bcd.c"

// VRAM update buffer
#include "vrambuf.h"
//#link "vrambuf.c"

// famitone2 library
//#link "famitone2.s"

// music and sfx
//#link "Bang.s"
extern char Bang_music_data[];

//#link "Bang_Game_over.s"
extern char Bang_Game_over_music_data[];

// Walking Metasprites
const unsigned char stand1[]={
  0,	0,	0xc4,	0,
  0,	8,	0xc4+1,	0,
  8,	0,	0xc4+2,	0,
  8,	8,	0xc4+3,	0,
128};

const unsigned char walk1[]={
  0,	0,	0xc8,	0,
  0,	8,	0xc8+1,	0,
  8,	0,	0xc8+2,	0,
  8,	8,	0xc8+3,	0,
128};

const unsigned char walk2[] ={
  0,	0,	0xe8,	0,
  0,	8,	0xe8+1,	0,
  8,	0,	0xe8+2,	0,
  8,	8,	0xe8+3,	0,
128};

const unsigned char walk3[]={
  0,	0,	0xcc,	0,
  0,	8,	0xcc+1,	0,
  8,	0,	0xcc+2,	0,
  8,	8,	0xcc+3,	0,
128};

const unsigned char walk1LEFT[]={
  8,	0,	0xc8,	(0)|OAM_FLIP_H,
  8,	8,	0xc8+1,	(0)|OAM_FLIP_H,
  0,	0,	0xc8+2,	(0)|OAM_FLIP_H,
  0,	8,	0xc8+3,	(0)|OAM_FLIP_H,
128};

const unsigned char walk2LEFT[]={
  8,	0,	0xe8,	(0)|OAM_FLIP_H,
  8,	8,	0xe8+1,	(0)|OAM_FLIP_H,
  0,	0,	0xe8+2,	(0)|OAM_FLIP_H,
  0,	8,	0xe8+3,	(0)|OAM_FLIP_H,
128};

const unsigned char walk3LEFT[]={
  8,	0,	0xcc,	(0)|OAM_FLIP_H,
  8,	8,	0xcc+1,	(0)|OAM_FLIP_H,
  0,	0,	0xcc+2,	(0)|OAM_FLIP_H,
  0,	8,	0xcc+3,	(0)|OAM_FLIP_H,
128};

// Misc. Item Metasprites

const unsigned char spike[]={
  0,	0,	0xd0,	2,
  0,	8,	0xd0+1,	2,
  8,	0,	0xd0+2,	2,
  8,	8,	0xd0+3,	2,
128};

const unsigned char block[]={
  0,	0,	0xd4,	1,
  0,	8,	0xd4+1,	1,
  8,	0,	0xd4+2,	1,
  8,	8,	0xd4+3,	1,
128};

// Metasprite Sequence
const unsigned char * const walkSeq[16] = {
  walk1LEFT, walk2LEFT, walk3LEFT,
  walk1LEFT, walk2LEFT, walk3LEFT,
  walk1LEFT, walk2LEFT,
  walk1, walk2, walk3,
  walk1, walk2, walk3,
  walk1, walk2
};

/*{pal:"nes",layout:"nes"}*/
const char PALETTE[32] = { 
  0x0C,			// screen color

  0x01,0x10,0x21,0x00,	// background palette 0
  0x1C,0x20,0x2C,0x00,	// background palette 1
  0x05,0x14,0x34,0x00,	// background palette 2
  0x06,0x16,0x26,0x00,   // background palette 3

  0x27,0x10,0x21,0x00,	// sprite palette 0
  0x01,0x3D,0x26,0x00,	// sprite palette 1
  0x3D,0x2D,0x16,0x00,	// sprite palette 2
  0x1D,0x32,0x24	// sprite palette 3
};

const char pal1[32] = {
  0x0c,
  0x1C,0x20,0x2C,0x00
};

const char pal2[32] = {
  0x0c,
  0x05,0x14,0x34,0x00,
};

const char pal3[32] = {
  0x0c,
  0x06,0x16,0x26,0x00
};

// Attribute table in PRG ROm
const char ATTRIBUTE_TABLE[0x40] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // rows 0-3
0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, // rows 4-7
0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, // rows 8-11
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // rows 12-15
0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, // rows 16-19
0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, // rows 20-23
0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, // rows 24-27
0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f // rows 28-29
};

// Provided by the Making Games for the NES book

// Define directive to create
// positioning arrays for our actors.
#define NUM_ACTORS 16
#define MAX_BLOCKS 16
#define grav 0.5

byte actor_x[NUM_ACTORS];
byte actor_y[NUM_ACTORS];
sbyte actor_dx[NUM_ACTORS];
sbyte actor_dy[NUM_ACTORS];
byte current_y = 180;

byte block_x[NUM_ACTORS];
byte block_y[NUM_ACTORS];
sbyte block_dx[NUM_ACTORS];

typedef struct {
  byte xpos;
  byte ypos;
  signed char dxpos;
  signed char dypos;
} softBlock;

typedef struct {
  byte xpos;
  byte ypos;
  signed char dxpos;
  signed char dypos;
} hardBlock;

typedef struct {
  byte xpos;
  byte ypos;
} spikes;

softBlock soft[MAX_BLOCKS];
hardBlock hard[MAX_BLOCKS];
spikes deadly[MAX_BLOCKS];
//softBlock* s = &soft[0];
//hardBlock* h = &hard[0];
spikes* d = &deadly[0];
bool jumping = false;
byte block_height;


// Global Zone above
//////////////////////////////////////////////////////////////////////////////////////////

void drawBlock(int x, int y)
{
  vram_adr(NTADR_A(x,y));
  vram_write("\x03", 1);
}

void titleScrn(void)
{
  int i;
  
  // The B.
  for(i = 10; i <=14; i++)
  {
    vram_adr(NTADR_A(8,i));
    vram_write("\x03", 1);
  }
  vram_adr(NTADR_A(9,10));
  vram_write("\x03", 2);
  vram_adr(NTADR_A(9,15));
  vram_write("\x03", 1);
  vram_adr(NTADR_A(10,14));
  vram_write("\x03", 1);
  vram_adr(NTADR_A(10,11));
  vram_write("\x03", 1);
  vram_adr(NTADR_A(9,12));
  vram_write("\x03", 1);
  for(i = 15; i >= 13; i--)
  {
   vram_adr(NTADR_A(10,i));
   vram_write("\x03", 2);
  }
  
  // The A.
  drawBlock(13, 12);
  drawBlock(13, 10);
  drawBlock(12, 11);
  drawBlock(14, 11);
  
  for(i = 12; i < 15; i++)
  {
    drawBlock(12, i);
  }
  for(i = 12; i < 15; i++)
  {
    drawBlock(14, i);
  }
  
  // The N
  //vram_adr(NTADR_A(16,10));
  //vram_write("\x03", 1);
  drawBlock(16, 10);
  for(i = 1; i <= 3; i++)
  {
    drawBlock(16+i, 10+i);
  }
  for(i = 1; i<=4; i++)
  {
    drawBlock(16, 10+i);    
  }
  drawBlock(20, 10);
  drawBlock(20, 11);
  drawBlock(20, 12);
  drawBlock(20, 13);
  drawBlock(20, 14);
  
  
  // The GEEEEEEEEEEEEEEE
  drawBlock(23, 10);
  drawBlock(23, 10);
  drawBlock(24, 10);
  drawBlock(22, 11);
  drawBlock(22, 12);
  drawBlock(22, 13);
  drawBlock(23, 14);
  drawBlock(24, 14);
  drawBlock(24, 12);
  drawBlock(25, 12);
  
  // TEEEEEXT
  vram_adr(NTADR_A(13, 16));
  vram_write("Soul Savior", 11);
  
  vram_adr(NTADR_A(11, 20));
  vram_write("Press Start", 11);
}

//could be a draw background function later
void NextScreen(void)
{
  
  int i;
  int j;
  ppu_off();
  for(i = 0; i <50; i++)
  {
    for(j = 0; j <50; j++){
      vram_adr(NTADR_A(i,j));
      vram_write("\x00", 1);
    }
    
  }
  
  
  //ppu_on_all();
  

}

void fade_in(void){
  int i;
  for(i = 0; i < 5; i++)
  {
    pal_bright(i);
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
  }
  
}

void fade_out(void)
{
  int i;
  for(i = 4; i >= 0; i--)
  {
    pal_bright(i);
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
    ppu_wait_frame();
  }
}

void flash(void)
{
  int i;
  for(i = 5; i<=8; i++)
  {
    pal_bright(i);
    ppu_wait_frame();
  }
}
  
void actorHandle(char num)
{
  char oam_id;
  oam_id = num;
  actor_x[num] -= 1;
  oam_id = oam_meta_spr(actor_x[num], actor_y[num], oam_id, walkSeq[7]);
}

void setup_guys()
{
  actor_x[0] = 60;
  actor_y[0] = current_y;
  actor_dx[0] = 0;
  actor_dy[0] = 0;
}

void setup_blocks()
{
 // byte i;
 // int x;
  
  hard[0].xpos = 60;
  hard[0].ypos = 180;
  hard[0].dxpos = 0;
  
  hard[1].xpos = 200;
  hard[1].ypos = 180;
  hard[1].dxpos = 0;
  
  
 //for(i = 2; i < 16; i++){
  //  x = i * 16;
  //  hard[i].xpos = x;
  //  hard[i].ypos = 196;
  //  hard[i].dxpos = 0;
  //}
  
 
  
  
  soft[0].xpos = 120;
  soft[0].ypos = 180;
  soft[0].dxpos = 0;
  
  soft[1].xpos = 180;
  soft[1].ypos = 180;
  soft[1].dxpos = 0;
  
  deadly[0].xpos = 140;
  deadly[0].ypos = 180;
}


void delete_blocks(void)
{
  char i;
  for(i = 0; i < MAX_BLOCKS; i++)
  {
    hard[i].xpos = 256;
    hard[i].ypos = -10;
    
    soft[i].xpos = 256;
    soft[i].ypos = -10;
    
    deadly[i].xpos = 256;
    deadly[i].ypos = -10;
  }
}


bool in_Hspace(hardBlock * h, byte px, byte py)
{
  if((px - h->xpos) < (byte)16 && (py - h->ypos) < (byte)16){
    return true;
  }
  else
    return false;
}

bool on_toph(hardBlock * h, byte px, byte py)
{
  if(py == h->ypos - 16 && in_Hspace(h, px, py + 16)){
    block_height = h->ypos;
    return true;
  }
  else
    return false;
}


bool in_Sspace(softBlock * s, byte px, byte py)
{
  if((px - s->xpos) < (byte)16 && (py - s->ypos) < (byte)16){     
    return true;
  }
  else
    return false;
}

bool on_tops(softBlock * s, byte px, byte py)
{ 
  if(py == s->ypos - 16 && in_Sspace(s, px, py + 16)){
    block_height = s->ypos;
    return true;
  }
  else
    return false; 
}

bool in_Dspace(spikes * d, byte px, byte py)
{
  if((px - d->xpos) < (byte)16 && (py - d->ypos) < (byte)16)
    return true;
  else
    return false;
}

bool on_topd(spikes * d, byte px, byte py)
{
  if(py == d->ypos - 16 && in_Dspace(d, px, py + 16))
    return true;
  else
    return false;
}



void gravity(void)
{
  byte i;
  for (i=0; i<2; i++) {
    hardBlock* h = &hard[i];
    softBlock* s = &soft[i];
  
    if(actor_y[0] == current_y)
    {
      actor_dy[0] = 0;
    }
    else if(on_toph(h, actor_x[0], actor_y[0]) || on_tops(s, actor_x[0], actor_y[0]))
    {
      actor_dy[0] = 0;
      current_y = block_height - 16;
      actor_y[0] = current_y;
    }
    else if(actor_y[0] < current_y - 16 && (!(on_toph(h, actor_x[0], actor_y[0])) || !(on_tops(s, actor_x[0], actor_y[0]))))
    {
      current_y = 180;
      actor_dy[0] = (1);
    }
  }
}

void scrollNext(void)
{
  int x = 0;
  int y = 0; 
  int dx = 1;
  int countdown = 220;
  oam_clear();
  while (countdown > 0)
  {
    ppu_wait_frame();
    x += dx;
    actorHandle(0);
    scroll(x, y);
    countdown--;
  }
  delete_blocks();
}

void wallCheck(void)
{
  if( actor_x[0] >= 235)
  {
    scrollNext();
  }
}
//function that returns what the player y is.
//are they on ground or box
int player_y(){
  int y = 180; //default y for ground
  return y;
}

void move_guys(char pad)
{
  //byte i;
  //for (i=0; i<2; i++) { This for loop would normally be what allows detection
                        // of multiple blocks. comment out and use the i to see.
  hardBlock* h = &hard[0];
  softBlock* s = &soft[0]; 
  
  
  if(pad&PAD_RIGHT && actor_x[0]<240 && !in_Hspace(h, actor_x[0] + 12, actor_y[0]) && !jumping)
  {
    if(!on_toph(h, actor_x[0], actor_y[0]) && !on_tops(s, actor_x[0], actor_y[0]) && current_y != 180){
      actor_dy[0] = 2;
      current_y = 180;
    }
    actor_dx[0] = 2;
  }
  else if(pad&PAD_LEFT && actor_x[0]>0 && !in_Hspace(h, actor_x[0], actor_y[0]) && !jumping)
  {
    if(!on_toph(h, actor_x[0], actor_y[0]) && !on_tops(s, actor_x[0], actor_y[0]) && current_y != 180){
      actor_dy[0] = 2;
      current_y = 180;
    }
    actor_dx[0] = -2;
  }
  else if(jumping){
    //added this because it would go to the else statement otherwise
  }
  else
  {
    actor_dx[0] = 0;
  }
  
  //////////////////////////////// JUMPING 
  
  if(pad&PAD_A && pad&PAD_RIGHT && actor_dy[0] == 0 && actor_x[0]<= 235 && !in_Hspace(h, actor_x[0], actor_y[0]))
  {
    jumping = true;
    actor_dy[0] = -12;
    //actor_x[0] = actor_x[0] + 4;
    actor_dx[0] = 2;
  }
  else if(pad&PAD_A && pad&PAD_LEFT && actor_dy[0] == 0 && actor_x[0]>=5 && !in_Hspace(h, actor_x[0], actor_y[0]))
  {
    jumping = true;
    actor_dy[0] = -12;
   // actor_x[0] = actor_x[0] - 4;
    actor_dx[0] = -2;
  }
  else if(pad&PAD_A && actor_y[0] == current_y)
  {
    jumping = true;
    actor_dy[0] = -12;
    actor_dx[0] = 0;
  }
  else if(actor_y[0] >= current_y && jumping)
  {
    jumping = false;
    actor_y[0] = current_y;
  }
  // END JUMPING 
  
  if(in_Hspace(h, actor_x[0], actor_y[0]))
  { 
    // Tryin to get it where you stand on top of box  
 
     if(h->xpos - actor_x[0] <= 16)
     {
            actor_y[0] = h->ypos - 16;
     }  
 
  }
  //} //This belongs to the for loop

}

void move_blocks(char pad)
{
  byte i;
  for (i=0; i<2; i++) {
    hardBlock* h = &hard[i];
    softBlock * s = &soft[i];
    if(pad&PAD_RIGHT && actor_x[0]<240 && in_Sspace(s, actor_x[0] + 12, actor_y[0]) && !jumping)
    {
      s->dxpos = 2;
    }
    else if(pad&PAD_LEFT && actor_x[0]>0 && in_Sspace(s, actor_x[0], actor_y[0]) && !jumping)
    {
      s->dxpos = -2;
    }
    else if(jumping){
      //added this because it would go to the else statement otherwise
    }
    else
    {
      s->dxpos = 0;
    }
  
  
  
    if((pad&PAD_RIGHT && actor_x[0]<240 && in_Sspace(s, h->xpos, h->ypos) && !jumping) || (pad&PAD_RIGHT && actor_x[0]<240 && in_Sspace(s, d->xpos, d->ypos) && !jumping))
    {
      s->dxpos = 0;
    }
    else if((pad&PAD_LEFT && actor_x[0]>0 && in_Sspace(s, h->xpos + 16, h->ypos) && !jumping) || (pad&PAD_LEFT && actor_x[0]>0 && in_Sspace(s, d->xpos + 16, d->ypos) && !jumping))
    {
      s->dxpos = 0;
    }
  }
  
}

void backgroundDisplay1() 
{
  vram_adr(NAMETABLE_A);
  vram_fill(0xd8, 32*24);
  vram_fill(0xdc, 4*32);
}

void backgroundDisplay2() 
{
  pal_bg(pal2);
  vram_adr(NAMETABLE_A);
  vram_fill(0xd9, 32*24);
  vram_fill(0xdc, 4*32);
}

void backgroundDisplay3() 
{
  pal_bg(pal1);
  vram_adr(NAMETABLE_A);
  vram_fill(0xdb, 32*24);
  vram_fill(0xdc, 4*32);
}

void backgroundDisplay4() 
{
  pal_bg(pal3);
  vram_adr(NAMETABLE_A);
  vram_fill(0xda, 32*24);
  vram_fill(0xdc, 4*32);
}



void gameOverScreen(void) 
{
  
  fade_out();
  music_pause(0);
  NextScreen();
  ppu_off();
  famitone_init(Bang_Game_over_music_data);
  oam_clear();
  ppu_on_all();

 
  vram_adr(NTADR_A(12, 14));
  vram_write("Game Over.", 9);
  fade_in();
  music_play(0);
  while(1)
  {
    
  }
}

int randNum()
{
  int n = rand() % 4 + 1;
  return n;
}

void chooseBackground(void)
{
  int n = randNum();
  switch(n)
  {
    case 0:
      backgroundDisplay1();
    case 1:
      backgroundDisplay2();
    case 2: 
      backgroundDisplay3();
    case 3:
      backgroundDisplay4();
  }
}

// setup PPU and tables
void setup_graphics() {
  // use to clear sprites
  oam_hide_rest(0);
  oam_clear();
  // set palette colors
  pal_all(PALETTE);
  //pal_bg(PALETTE);
  // enable rendering
  ppu_on_all();
  fade_in();
}

void postStart(){
  fade_in();
}

// set up famitone library
void setup_sounds() {
  famitone_init(Bang_music_data);
  
 // sfx_init(demo_sounds);
  nmi_set_callback(famitone_update);
}

void main(void)
{
  bool start = false;
  char pad;
  char oam_id;
  byte animSeq;
  
  byte i;

  titleScrn();
  
  setup_graphics();
  
  while(!start)
  {  
    pad = pad_poll(0);
    if(pad&PAD_START){
      start = true;
      flash();
      fade_out();
    }
  }
  NextScreen();
  
  // Set up guy
  setup_guys();
  
  setup_blocks();
 
  setup_sounds();
  
  backgroundDisplay4(); 
  
  ppu_on_all();
  
  postStart();
  music_play(0);
  while(1) 
  {
    // General game structure should follow standard game loop.
    // 1. Update
    oam_id = 0;
    pad = pad_poll(0);
    move_guys(pad);
    move_blocks(pad);
    animSeq = actor_x[0] & 7;
    if (actor_dx[0] >= 0)
    {
      animSeq += 8;
    }
    // 2. Reflect & Draw Updates
    oam_id = oam_meta_spr(actor_x[0], actor_y[0], oam_id, walkSeq[animSeq]);
    
    for(i = 0; i < 2; i++){
      oam_id = oam_meta_spr(hard[i].xpos, hard[i].ypos, oam_id, block);
      
    }
    
    for(i = 0; i < 2; i++){
      oam_id = oam_meta_spr(soft[i].xpos, soft[i].ypos, oam_id, block);
      
    }
    
    
    
    
    
    
    oam_id = oam_meta_spr(deadly[0].xpos, deadly[0].ypos, oam_id, spike);
    
    actor_x[0] += actor_dx[0];
    actor_y[0] += actor_dy[0];
    for (i=0; i<MAX_BLOCKS; i++) {
      softBlock * s = &soft[i];
      s->xpos += s->dxpos;
    }
    gravity();
    wallCheck();
    
    if(on_topd(d, actor_x[0], actor_y[0]) || in_Dspace(d, actor_x[0], actor_y[0])){
      gameOverScreen();
      
    }
    // 3. Return to Update

    ppu_wait_frame();
  }
  
}







