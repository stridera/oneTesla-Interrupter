/*
 * Licensed under the GNU GPL 3.0
 * Copyleft 2015 oneTesla, LLC
 */

#include "improv/include/MidiFile.h"

using namespace std;

#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define MAX_EVENTS 2000

typedef unsigned char uchar;

typedef struct {
	double time;
	uchar cmd;
	uchar db1;
	uchar db2;
} ftMidiEvent;

typedef struct {
	uchar b1;
	uchar b2;
	uchar db1;
	uchar db2;
} pMidiEvent;

double  tempo = 60.0;

int convert(MidiFile& midifile);
void setTempo(MidiFile& midifile, int index, double& tempo);

static ftMidiEvent *events[MAX_EVENTS];
static pMidiEvent *p_events[MAX_EVENTS];

int main(int argc, char* argv[]) {
	if (argc <= 1) {
	   cout << "Usage: " << argv[0] << " <file name> [arguments]" << endl;
	   cout << "Arguments:" << endl;
	   cout << "-v: verbose mode" << endl;
	   return 1;
   }
   int i;
   FILE *out;
   FILE *in = fopen(argv[1], "r");
   int verbose = 0;
   if (argc > 2) {
	   if (argv[2][0] == '-' && argv[2][1] == 'v') {
		   verbose = 1;
	   }
   }
   if (in == NULL) {
	   cout << "Error opening file " << argv[1] << endl;
	   return 1;
   }
   fclose(in);
   for (int i = 0; i < MAX_EVENTS; i++) {
	   events[i] = (ftMidiEvent*) malloc(sizeof(ftMidiEvent));
   }

   for (i = 0; i < MAX_EVENTS; i++) {
	   p_events[i] = (pMidiEvent*) malloc(sizeof(pMidiEvent));
   }

   int len = strlen(argv[1]);
   char* outname = (char*) malloc(sizeof(char)*(len+5));
   for (i = 0; i < len; i++) {;
	   if (argv[1][i] == '.') break;
	   outname[i] = argv[1][i];
   }
   outname[i] = '.';
   outname[i+1] = 'O';
   outname[i+2] = 'M';
   outname[i+3] = 'D';
   outname[i+4] = '\0';

   MidiFile midifile(argv[1]);
   out = fopen(outname, "wb");
   
   int count = convert(midifile);

   if (verbose) cout << count << " MIDI events in file" << endl;

   double last_time = 0;
   int t_len = (int) (1000 * events[count - 1]->time);

   unsigned char tbuf[4] = {(t_len & 0xff000000) >> 24, (t_len & 0x00ff0000) >> 16, (t_len & 0x0000ff00) >> 8, t_len & 0x000000ff};
   fwrite(tbuf, 1, 4, out);

   for (i = 0; i < count; i++) {
	   if (events[i]->cmd == 9 && events[i]->db2 != 0) {
	     for (int j = i; j < count; j++) {
			if ((events[j]->cmd == 8 || events[j]->db2 == 0) && (events[j]->time - events[i]->time) < 0.0001
				&& events[j]->db1 == events[i]->db1) {
				unsigned char tmp = events[i]->db2;
				events[i]->cmd = events[j]->cmd;
				events[i]->db2 = events[j]->db2;
				events[j]->cmd = 0x09;
				events[j]->db2 = tmp;
				break;
			}
		 }
	   }
   }

 

   for (i = 0; i < count; i++) {
	   ftMidiEvent* f = events[i];
	   double delta_t = f->time - last_time;
	   last_time = f->time;
	   if (delta_t < 0) {
		   cout << "Error: out-of-order file" << endl;
		   return 1;
	   }
	   unsigned int delta_t_ms = (unsigned int) (1000 * delta_t);
	   if (f->cmd == 9 && f->db2 != 0) {
		   if (verbose) cout << f->time << " note "<< (unsigned int) f->db1 << " on" << endl;
	   }
	   if (f->cmd == 8 || (f->cmd == 9 && f->db2 == 0)) {
		   if (verbose) cout << f->time << " note "<< (unsigned int) f->db1 << " off" << endl;
	   }
	   p_events[i]->b1 = (uchar) ((delta_t_ms & 0x7f80) >> 7);
	   p_events[i]->b2 = (uchar) (delta_t_ms & 0x007f);
	   p_events[i]->b2 |= (uchar) ((f->cmd & 0x01) << 7);
	   p_events[i]->db1 = f->db1;
	   p_events[i]->db2 = f->db2;
   }
   for (i = 0; i < count; i++) {
	   pMidiEvent *p = p_events[i];
	   unsigned char bytes[4] = {p->b1, p->b2, p->db1, p->db2};
	   fwrite(bytes, 1, 4, out);
	   
   }
   unsigned int t_total = 0;
   for (i = 0; i < count; i++) {
	   pMidiEvent *p = p_events[i];
	   t_total += ((((unsigned int) p->b1) << 7) + (p->b2 & 0x7f));
   }
   fclose(out);
   return 0;
}

int convert(MidiFile& midifile) {
   midifile.absoluteTime();
   midifile.joinTracks();
   int evIndex = 0;
   for (int i=0; i<midifile.getNumEvents(0); i++) {
	  if(evIndex == MAX_EVENTS) return MAX_EVENTS;
      uchar command = midifile.getEvent(0, i).data[0] & 0xf0;
	  uchar ch = midifile.getEvent(0, i).data[0] & 0x0f;
      if ((command == 0x90 || command == 0x80) && ch == 0) {
         events[evIndex]->cmd = command >> 4;
         events[evIndex]->db1 = midifile.getEvent(0, i).data[1];
		 events[evIndex]->db2 = midifile.getEvent(0, i).data[2];
         events[evIndex]->time = midifile.getEvent(0, i).time * 60.0 / tempo / 
               midifile.getTicksPerQuarterNote();
		 evIndex++;
      }
      if (midifile.getEvent(0, i).data[0] == 0xff &&
                 midifile.getEvent(0, i).data[1] == 0x51) {
         setTempo(midifile, i, tempo);
      }
   }
   return evIndex;
}

void setTempo(MidiFile& midifile, int index, double& tempo) {
   double newtempo = 0.0;
   static int count = 0;
   count++;

   MFEvent& mididata = midifile.getEvent(0, index);

   int microseconds = 0;
   microseconds = microseconds | (mididata.data[3] << 16);
   microseconds = microseconds | (mididata.data[4] << 8);
   microseconds = microseconds | (mididata.data[5] << 0);

   newtempo = 60.0 / microseconds * 1000000.0;
   if (count <= 1) {
      tempo = newtempo;
   } else if (tempo != newtempo) {
      cout << "; WARNING: change of tempo from " << tempo 
           << " to " << newtempo << " ignored" << endl;
   }
}
