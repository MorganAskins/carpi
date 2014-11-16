/* 
 * state-utils
 * (c) 2005-2009 by Avuton Olrich <avuton@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libmpdclient.h"
#include "conn.h"
#include "state.h"

/*
 * void exit_on_help(char ** argv):
 * Takes argv as an argument
 * 
 * Expected action: Simply check for -h or --help as an argument
 * and if it exists show usage.
 * 
 * Returns: exits only
 * 
 */
static void exit_on_help(char **argv)
{
  if (!(argv[1] && argv[1][0] == '-'))
    return;

  if ((argv[1][1] == '-' && argv[1][2] == 'h') || (argv[1][1] == 'h')) {
    printf("Usage: state-save [STATE NAME]"
	   "\nExample: state-save my_cool_statefile"
	   "\n\n"
	   "\n  -h, --help\t\tThis usage"
	   "\n\nReport bugs to Avuton Olrich <avuton@gmail.com>"
	   "\nSee 'man 1 state-save' for more information\n");
    exit(1);
  }

}

/*
 * FILE * get_fp(int,char):
 * Takes argc,argv as it's arguments.
 * 
 * Expected action: This simply formats the filename, gets
 * the current user's home directory name, if it doesn't yet
 * exist it creats it, then turns the filename into a file 
 * pointer.
 * 
 * Returns: A file pointer
 */
static FILE *get_fp(int argc, char **argv)
{
  /* char filename[FILENAME_MAX]; */
  /* char dirname[FILENAME_MAX]; */
  /* FILE *fp; */

  /* sprintf(dirname, "%s/.mpd_states", get_home_dir()); */

  /* if (argc == 1) { */
  /* 	sprintf(filename, "%s/default", dirname); */
  /* } else { */
  /* 	sprintf(filename, "%s/%s", dirname, argv[1]); */
  /* } */

  /* /\* */
  /*  * Create the directory/file if necessary and  */
  /*  *  make sure things are coherant. */
  /*  *\/ */
  /* errno = 0; */
  /* while (!(fp = fopen(filename, "w"))) { */
  /* 	/\* ENOENT: No such file or directory *\/ */
  /* 	if (errno == ENOENT) { */
  /* 		errno = 0; */

  /* 		if (mkdir(dirname, (mode_t) 0700)) { */
  /* 			break; */
  /* 		} */
  /* 	} */
  /* 	check_perror(errno, "Couldn't create state-utils directory"); */
  /* } */
  /* return fp; */
  int str_length = 256;
  char* filename = "/var/lib/mpd/mpdstate_tmp";
  
  FILE* fp;
  fp = fopen(filename, "w");

  return fp;
}

/*
 * void print_playlist(FILE,conn):
 * Takes a file pointer and the connection as arguments
 * 
 * Expected action: Print the playlist from connection
 * to the file pointer
 * 
 * Returns: void, There's no need to do anything other
 * than log the error and return.
 * 
 * Example output:
 * playlist_begin
 * 0:Albums/Sublime/40 Oz. To Freedom/01.sublime_-_waiting_for_my_ruca.flac
 * 1:Albums/Sublime/40 Oz. To Freedom/02.sublime_-_40_oz._to_freedom.flac
 * playlist_end
 * 
 */
static void print_playlist(FILE * fp, mpd_Connection * conn)
{

  mpd_InfoEntity *entity = NULL;
  mpd_sendPlaylistInfoCommand(conn, -1);

  entity = mpd_getNextInfoEntity(conn);

  /*
   * Don't freak if we can't get playlist
   * user may simply not have permissions.
   */
  if (conn->error || !entity) {
    fprintf(stderr, "Cannot get the playlist, continuing...\n");
    mpd_clearError(conn);
    return;
  }

  fprintf(fp, "playlist_begin\n");
  do {
    if (entity->type == MPD_INFO_ENTITY_TYPE_SONG) {
      fprintf(fp, "%i:%s\n",
	      entity->info.song->pos,
	      entity->info.song->file);
    }
    mpd_freeInfoEntity(entity);
  } while ((entity = mpd_getNextInfoEntity(conn)));
  mpd_finishCommand(conn);
  fprintf(fp, "playlist_end\n");
  printErrorAndExit(conn);
}

/*
 * void print_outputs(FILE,conn)
 * Takes a file pointer and the connection as arguments
 * 
 * Expected action: Print the outputs from the connection to
 * the file pointer.
 * 
 * Returns: void, There's no need to do anything other than log the
 * error and return.
 * 
 * Example output:
 * outputs_begin
 * 0:1:ALSA output (emu10k1)
 * 1:1:ALSA output (intel8x0)
 * outputs_end
 * 
 * Explanation of output:
 * The first number is the device number, sequentially.
 * The second number is 0 is disabled, 1 is enabled.
 * The third is the name as defined in the MPD.
 */
static void print_outputs(FILE * fp, mpd_Connection * conn)
{
  mpd_OutputEntity *output = NULL;
  mpd_sendOutputsCommand(conn);

  output = mpd_getNextOutput(conn);

  /*
   * Don't freak if we can't get outputs
   * user may simply not have permissions.
   */
  if (conn->error || !output) {
    if (conn->error) {
      fprintf(stderr,
	      "Cannot get the outputs due to: %s, continuing...\n",
	      conn->errorStr);
    } else {	/* !output */
      fprintf(stderr,
	      "Cannot get the outputs, this is not necessarily an error, continuing...\n");
    }
    mpd_clearError(conn);
    return;
  }

  fprintf(fp, "outputs_begin\n");
  do {
    if (output->id >= 0) {
      fprintf(fp, "%i:%i:%s\n",
	      output->id, output->enabled, output->name);
    }
    mpd_freeOutputElement(output);
  } while ((output = mpd_getNextOutput(conn)));
  mpd_finishCommand(conn);
  fprintf(fp, "outputs_end\n");
  printErrorAndExit(conn);
}

/*
 * void print_status(FILE,conn)
 * Takes a file pointer and connection as arguments
 * 
 * Expected action: Get all useful information from the MPD
 * status conn and output to the file pointer.
 * 
 * Returns: void, There's no need to do anything other than log
 * or exit() if required.
 * 
 * Example output:
 * state: play
 * current: 5
 * time: 62
 * random: 0
 * repeat: 0
 * crossfade: 0
 * volume: 100
 */
static void print_status(FILE * fp, mpd_Connection * conn)
{
  mpd_Status *status;

  mpd_sendStatusCommand(conn);
  printErrorAndExit(conn);

  status = mpd_getStatus(conn);
  printErrorAndExit(conn);

  if (!status) {
    fprintf(stderr, "Out of memory");
    exit(8);
  }

  mpd_nextListOkCommand(conn);
  printErrorAndExit(conn);

  /* state: play */
  fprintf(fp, "state: ");
  if (status->state == MPD_STATUS_STATE_PLAY) {
    fprintf(fp, "play\n");
  } else if (status->state == MPD_STATUS_STATE_PAUSE) {
    fprintf(fp, "pause\n");
  } else {
    fprintf(fp, "stop\n");
  }

  /* current: 0 */
  fprintf(fp, "current: %i\n", status->song);

  /* time: 1029 */
  fprintf(fp, "time: %i\n", status->elapsedTime);

  /* random: 0 */
  fprintf(fp, "random: %i\n", status->random ? 1 : 0);

  /* repeat: 0 */
  fprintf(fp, "repeat: %i\n", status->repeat ? 1 : 0);

  if (status->single) {
    /* single: 0 */
    fprintf(fp, "single: %i\n", status->single ? 1 : 0);
  }

  if (status->consume) {
    /* consume: 0 */
    fprintf(fp, "consume: %i\n", status->consume ? 1 : 0);
  }

  /* crossfade: 0 */
  fprintf(fp, "crossfade: %i\n", status->crossfade ? 1 : 0);

  fprintf(fp, "volume: %i\n", status->volume);

  mpd_finishCommand(conn);

  mpd_freeStatus(status);
  printErrorAndExit(conn);
}

/*
 * duh
 */
// The loop
void run_file(int argc, char* argv[])
{
  FILE *fp;
  mpd_Connection *conn;

  exit_on_help(argv);
  conn = setup_connection(NULL, NULL);
  fp = get_fp(argc, argv);

  print_status(fp, conn);
  print_playlist(fp, conn);
  print_outputs(fp, conn);

  mpd_closeConnection(conn);
  fclose(fp);
  
  return;
}

void copy_file(int argc, char* argv[])
{
  char* tmp_name = "/var/lib/mpd/mpdstate_tmp";
  char* cpy_name = "/var/lib/mpd/mpdstate";

  // Read tmp file and make sure non-zero size;
  FILE* infile = fopen(tmp_name, "r");
  // File size:
  fseek(infile, 0, SEEK_END);
  long lSize = ftell(infile);
  rewind(infile);

  if( lSize == 0 )
  {
    fclose(infile);
    return;
  }

  // Write to real file if non-zero
  FILE* outfile = fopen(cpy_name, "w");
  char buffer[1];
  while(fread(buffer, sizeof(buffer), 1, infile))
    fwrite(buffer, sizeof(buffer), 1, outfile);
  fclose(infile);
  fclose(outfile);
  
  return;
}

int main(int argc, char **argv)
{
  unsigned int delay = 1 * 1000000; /* microseconds */
  for(;;)
  {
    usleep(delay);
    run_file(argc, argv);
    copy_file(argc, argv);
  }
  return 0;
}
