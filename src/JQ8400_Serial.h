/** 
 * Arduino Library for JQ8400 MP3 Module
 * 
 * Copyright (C) 2019 James Sleeman, <http://sparks.gogo.co.nz/jq6500/index.html>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
 * THE SOFTWARE.
 * 
 * @author James Sleeman, http://sparks.gogo.co.nz/
 * @license MIT License
 * @file
 */

#ifndef JQ8400Serial_h
#define JQ8400Serial_h

#include <SoftwareSerial.h>

#define MP3_EQ_NORMAL     0
#define MP3_EQ_POP        1
#define MP3_EQ_ROCK       2
#define MP3_EQ_JAZZ       3
#define MP3_EQ_CLASSIC    4

#define MP3_SRC_USB       0
#define MP3_SRC_SDCARD    1
#define MP3_SRC_FLASH     2
#define MP3_SRC_BUILTIN   MP3_SRC_FLASH

// Looping options, ALL, FOLDER, ONE and ONE_STOP are the 
// only ones that appear to do much interesting
//  ALL plays all the tracks in a repeating loop
//  FOLDER plays all the tracks in the same folder in a repeating loop
//  ONE plays the same track repeating
//  ONE_STOP does not loop, plays the track and stops
//  RAM seems to play one track and someties disables the ability to 
//  move to next/previous track, really weird.

#define MP3_LOOP_ALL      0
#define MP3_LOOP_ONE      1
#define MP3_LOOP_ONE_STOP 2
#define MP3_LOOP_RANDOM   3
#define MP3_LOOP_FOLDER   4
#define MP3_LOOP_RANDOM_RANDOM   5
#define MP3_LOOP_FOLDER_STOP     6
#define MP3_LOOP_ALL_STOP        7

#define MP3_LOOP_NONE            0 

#define MP3_STATUS_STOPPED 0
#define MP3_STATUS_PLAYING 1
#define MP3_STATUS_PAUSED  2

// The response from a status query could be unreliable
//  we can increase this to check multiple times.
#define MP3_STATUS_CHECKS_IN_AGREEMENT 1

#define MP3_DEBUG 1

#define HEX_PRINT(a) if(a < 16) Serial.print(0); Serial.print(a, HEX);

class JQ8400_Serial : public SoftwareSerial
{
  
  public: 

    /** Create JQ8400 object.
     * 
     * Example, create global instance:
     * 
     *     JQ8400_Serial mp3(8,9);
     * 
     * For a 5v Arduino:
     * -----------------
     *  * TX on JQ8400 connects to D8 on the Arduino
     *  * RX on JQ8400 connects to one end of a 1k resistor,
     *      other end of resistor connects to D9 on the Arduino
     * 
     * For a 3v3 Arduino:
     * -----------------
     *  * TX on JQ8400 connects to D8 on the Arduino
     *  * RX on JQ8400 connects to D9 on the Arduino
     * 
     * Of course, power and ground are also required, VCC on JQ8400 is 5v tolerant (but RX isn't totally, hence the resistor above).
     * 
     * And then you can use in your setup():
     * 
     *     mp3.begin(9600)
     *     mp3.reset();
     *
     * and all the other commands :-)
     */
    
    JQ8400_Serial(short rxPin, short txPin) : SoftwareSerial(rxPin,txPin) { };
    
    /** Start playing the current file, if paused the playing is resumed.
     * 
     *  If stopped or playing the playing is started from beginning.
     * 
     */
    
    void play();
    
    /** Restart the current track from the beginning.
     * 
     */
    
    void restart();
    
    /** Pause the current file.  To unpause, use play(),
     *  to unpause and go back to beginning of track use restart()
     */
    
    void pause();
    
    /** Stop the current playing (if any).
     */
    
    void stop();
    
    /** Play the next file.
     */
    
    void next();
    
    /** Play the previous file.
     */
    
    void prev();
    
    /** Play the next folder. 
     */
    
    void nextFolder();
    
    /** Play the previous folder. 
     */
    
    void prevFolder();
    
    /** Play a specific file based on it's (FAT table) index number.
     * 
     *  Note that the index number has nothing to do with the file name (except if you 
     *  uploaded/copied them to the media in order of file name).
     * 
     *  To sort your SD Card FAT table, search for a FAT sorting utility for your operating system 
     *  of choice.  Specifically on Linux fatsort is the tool you want but be sure to get a recent
     *  version which supports FAT-12 that the JQ8400 use for flash memory.
     * 
     *  https://sourceforge.net/projects/fatsort/
     * 
     *  @param fileNumber FAT Table index of the file to play next.
     */
    
    void playFileByIndexNumber(unsigned int fileNumber);        
    
    /** Play a specific file in a specific folder based on the name of those folder and file.
     *
     * To use this function, folders must be named from 00 to 99, and the files in those folders
     * must be named from 000.mp3 to 999.mp3
     * 
     * So to play the file "/03/006.mp3" use mp3.playFileNumberInFolderNumber(3, 6);
     * 
     * Note that zero padding of your file names is required - "01/002.mp3" good, "1/2.mp3" bad.
     * 
     */
    
    void playFileNumberInFolderNumber(unsigned int folderNumber, unsigned int fileNumber);
    
    /** Seek to a specific file based on it's (FAT table) index number.  
     * 
     *  The file will not start playing until you issue `play()`
     * 
     *  Note that any currently playing file will stop immediately.
     * 
     *  It seems (undocumented) that seeking to a track will wake up the device's output
     *   and keep it awake until you play or stop (or it stops after playing), you may wish
     *   to issue a sleep() after seeking.
     * 
     *  Note that the index number has nothing to do with the file name (except if you 
     *  uploaded/copied them to the media in order of file name).
     * 
     *  To sort your SD Card FAT table, search for a FAT sorting utility for your operating system 
     *  of choice.  Specifically on Linux fatsort is the tool you want but be sure to get a recent
     *  version which supports FAT-12 that the JQ8400 use for flash memory.
     * 
     *  https://sourceforge.net/projects/fatsort/
     * 
     *  @param fileNumber FAT Table index of the file to play next.
     */
    
    void seekFileByIndexNumber(unsigned int fileNumber);
    
    /** Increase the volume by 1 (volume ranges 0 to 30). */
    
    void volumeUp();
    
    /** Decrease the volume by 1 (volume ranges 0 to 30). */
    
    void volumeDn();
    
    /** Set the volume to a specific level (0 to 30). 
     * 
     * @param volumeFrom0To30 Level of volume to set from 0 to 30
     */
    
    void setVolume(byte volumeFrom0To30);
    
    /** Set the equalizer to one of 6 preset modes.
     * 
     * @param equalizerMode One of the following, 
     * 
     *  *  MP3_EQ_NORMAL
     *  *  MP3_EQ_POP        
     *  *  MP3_EQ_ROCK       
     *  *  MP3_EQ_JAZZ       
     *  *  MP3_EQ_CLASSIC    
     * 
     */
    
    void setEqualizer(byte equalizerMode); // EQ_NORMAL to EQ_BASS
    
    /** Set the looping mode.
     * 
     * @param loopMode One of the following,
     * 
     *  *  MP3_LOOP_ALL       - Loop through all files.
     *  *  MP3_LOOP_FOLDER    - Loop through all files in the same folder (SD Card only)
     *  *  MP3_LOOP_ONE       - Loop one file.
     *  *  MP3_LOOP_RAM       - Loop one file (uncertain how it is different to the previous!)
     *  *  MP3_LOOP_NONE      - No loop, just play one file and then stop. (aka MP3_LOOP_ONE_STOP)
     */
    
    void setLoopMode(byte loopMode);
    
    /** Set the source to read mp3 data from.  Note that the datasheet calls this "drive".
     * 
     *  @param source One of the following...
     * 
     *   * MP3_SRC_BUILTIN    - Files read from the on-board flash memory
     *   * MP3_SRC_SDCARD     - Files read from the SD Card
     *   * MP3_SRC_USB        - Files from a connected USB device?  I have not seen modules capable of this, but possible?
     */
    
    void setSource(byte source);        // SRC_BUILTIN or SRC_SDCARD       
    
    /** Return the currently selected source.
     * 
     *  @return One of the following...
     * 
     *   * MP3_SRC_BUILTIN    - Files read from the on-board flash memory
     *   * MP3_SRC_SDCARD     - Files read from the SD Card
     *   * MP3_SRC_USB        - Files from a connected USB device?  I have not seen modules capable of this, but possible?
     */
    
    uint8_t getSource();
        
    /** Return boolean indicating if the given source is available (can be selected using `setSource()`)
     * 
     * @param  source One of the following     
     *   * MP3_SRC_BUILTIN    - Files read from the on-board flash memory
     *   * MP3_SRC_SDCARD     - Files read from the SD Card
     *   * MP3_SRC_USB        - Files from a connected USB device?  I have not seen modules capable of this, but possible?
     * 
     * @return bool
     */
    
    uint8_t isSourceAvailable(uint8_t source)
    {
      return getAvailableSources() & 1<<source;
    }
    
    /** Put the device to sleep.
     *
     *  This will stop all playing.  When you play() again it will be 
     *  from the beginning of the current track.
     * 
     */
    
    void sleep();
    
    /** Reset the device (softly).
     *       
     * It may be necessary in practice to actually power-cycle the device
     * in case it gets confused or something.
     * 
     * So if designing a PCB/circuit including JQ8400 modules it might be 
     * worth while to include such ability (ie, power the device through 
     * a MOSFET which you can turn on/off at will).
     * 
     */
    
    void reset();
    
    // Status querying commands
    /** Get the status from the device.
     * 
     * CAUTION!  This is somewhat unreliable for the following reasons...
     * 
     *  1. When playing from the on board memory (MP3_SRC_BUILTIN), STOPPED sems
     *     to never be returned, only PLAYING and PAUSED
     *  2. Sometimes PAUSED is returned when it is PLAYING, to try and catch this
     *     getStatus() actually queries the module several times to ensure that
     *     it is really sure about what it tells us.
     *
     * @return One of MP3_STATUS_PAUSED, MP3_STATUS_PLAYING and MP3_STATUS_STOPPED
     */
    
    byte getStatus();
    
    uint8_t busy() { return getStatus() == MP3_STATUS_PLAYING; }
    
    /** Get the current volume level.
     * 
     * @return Value between 0 and 30
     */
    
    byte getVolume();
    
    /** Get the equalizer mode.
     * 
     * @return One of the following, 
     * 
     *  *  MP3_EQ_NORMAL
     *  *  MP3_EQ_POP        
     *  *  MP3_EQ_ROCK       
     *  *  MP3_EQ_JAZZ       
     *  *  MP3_EQ_CLASSIC    
     *  *  MP3_EQ_BASS      
     */
    
    byte getEqualizer();
    
    /** Get loop mode.
     * 
     * @return One of the following,
     * 
     *  *  MP3_LOOP_ALL       - Loop through all files.
     *  *  MP3_LOOP_FOLDER    - Loop through all files in the same folder (SD Card only)
     *  *  MP3_LOOP_ONE       - Loop one file.
     *  *  MP3_LOOP_RAM       - Loop one file (uncertain how it is different to the previous!)
     *  *  MP3_LOOP_NONE      - No loop, just play one file and then stop. (aka MP3_LOOP_ONE_STOP)
     */
    
    byte getLoopMode();
    
    
    /** Count the number of files on the current media.
     * 
     * @return Number of files present on that media.
     * 
     */
    
    unsigned int   countFiles();    
    
    /** Count the number of files on the specified media.
     * 
     *  Note: changes to the given source.
     * 
     * @param source One of MP3_SRC_BUILTIN and MP3_SRC_SDCARD
     * @return Number of files present on that media.
     * 
     */
    
    unsigned int   countFiles(byte source) { setSource(source); return countFiles(); };
    
    /** Count the number of folders on the current media.
     * 
     * @return Number of folders present on that media.
     */
    
    unsigned int   countFolders();    
    
    /** Count the number of folders on the specified media.
     * 
     * Note: Changes to the specified source
     * 
     * @param source One of MP3_SRC_BUILTIN and MP3_SRC_SDCARD
     * @return Number of folders present on that media.
     */
    
    unsigned int   countFolders(byte source) { setSource(source); return countFolders(); };
    
    /** For the currently playing (or paused, or file that would be played 
     *  next if stopped) file, return the file's (FAT table) index number.
     * 
     *  This number can be used with playFileByIndexNumber();
     * 
     *  @return Number of file.
     */
    
    unsigned int   currentFileIndexNumber();
    
    /** For the currently playing (or paused, or file that would be played 
     *  next if stopped) file, return the file's (FAT table) index number.
     * 
     *  This number can be used with playFileByIndexNumber();
     * 
     *  @deprecated Use `currentFileIndexNumber()` instead with no parameters.
     *  @param  source One of MP3_SRC_BUILTIN and MP3_SRC_SDCARD, this parameter is ignored.
     *  @return Number of file.
     */
    
    unsigned int   currentFileIndexNumber(byte source __attribute__((unused)) ) { return currentFileIndexNumber(); }
 
    /** For the currently playing or paused file, return the 
     *  current position in seconds.
     * 
     * @return Number of seconds into the file currently played.
     * 
     */
    unsigned int   currentFilePositionInSeconds();
    
    /** For the currently playing or paused file, return the 
     *  total length of the file in seconds.
     * 
     * @return Length of audio file in seconds.
     */
    
    unsigned int   currentFileLengthInSeconds();
    
    /** Get the name of the "current" file on the SD Card.
     *  
     * The current file is the one that is playing, paused, or if stopped then
     * could be next to play or last played, uncertain.
     * 
     * It would be best to only consult this when playing or paused
     * and you know that the SD Card is the active source.
     * 
     * Unfortunately there is no way to query the device to find out
     * which media is the active source (at least not that I know of).
     * 
     */
    
    void           currentFileName(char *buffer, unsigned int bufferLength);    
        
    
  protected:
    
    /** Send a command to the JQ8400 module, 
     * @param command        Byte value of to send as from the datasheet.
     * @param arg1           First (if any) argument byte
     * @param arg2           Second (if any) argument byte
     * @param responseBuffer Buffer to store a single line of response, if NULL, no response is read.
     * @param buffLength     Length of response buffer including NULL terminator.
     */
    
    void sendCommand(byte command, uint8_t arg1, uint8_t arg2, char *responseBuffer, unsigned int bufferLength);

    void sendCommandData(byte command, uint8_t *requestBuffer, uint8_t requestLength, uint8_t *responseBuffer, unsigned int bufferLength);
    
    // Just some different versions of that for ease of use
    void sendCommand(byte command);    
    void sendCommand(byte command, byte arg1);    
    void sendCommand(byte command, byte arg1, byte arg2);
    
    /** Send a command to the JQ8400 module, and get a 16 bit response.
     * 
     * @param command        Byte value of to send as from the datasheet.
     * @return Response from module.
     */
    
    unsigned int sendCommandWithUnsignedIntResponse(byte command);

    /** Send a command to the JQ8400 module, and get an 8 bit response.
     * 
     * @param command        Byte value of to send as from the datasheet.
     * @return Response from module.
     */
    
    uint8_t sendCommandWithByteResponse(uint8_t command);

    
    
    /** Return a bitmask of the available sources.
     */
    
    uint8_t getAvailableSources();
    
    size_t readBytesUntilAndIncluding(char terminator, char *buffer, size_t length, byte maxOneLineOnly = 0);
    
    int    waitUntilAvailable(unsigned long maxWaitTime = 1000);
    
    static const uint8_t MP3_CMD_BEGIN = 0xAA;
    
    static const uint8_t MP3_CMD_PLAY = 0x02;
    static const uint8_t MP3_CMD_PAUSE = 0x03;
    
    static const uint8_t MP3_CMD_STOP = 0x10; // Not sure, maybe 0x04?
    
    
    static const uint8_t MP3_CMD_NEXT = 0x06;
    static const uint8_t MP3_CMD_PREV = 0x05;
    static const uint8_t MP3_CMD_PLAY_IDX = 0x07;
    static const uint8_t MP3_CMD_SEEK_IDX = 0x1F;
    static const uint8_t MP3_CMD_INSERT_IDX = 0x16; // FIXME - Implement
    
    
    static const uint8_t MP3_CMD_NEXT_FOLDER = 0x0F;
    static const uint8_t MP3_CMD_PREV_FOLDER = 0x0E;
    
      static const uint8_t MP3_CMD_PLAY_FILE_FOLDER = 0x08; // FIXME
    
    static const uint8_t MP3_CMD_VOL_UP = 0x14;
    static const uint8_t MP3_CMD_VOL_DN = 0x15;
    static const uint8_t MP3_CMD_VOL_SET = 0x13;
    
    static const uint8_t MP3_CMD_EQ_SET = 0x1A;
    static const uint8_t MP3_CMD_LOOP_SET = 0x18;    
    static const uint8_t MP3_CMD_SOURCE_SET = 0x0B;
    
    static const uint8_t MP3_CMD_SLEEP = 0x04;    // I am not sure about these, see implmentation of sleep() and reset()
    static const uint8_t MP3_CMD_RESET = 0x04;    //  what I have done seems to work maybe, maybe.

    static const uint8_t MP3_CMD_STATUS = 0x01;
    
    static const uint8_t MP3_CMD_GET_SOURCES = 0x09;
    static const uint8_t MP3_CMD_GET_SOURCE  = 0x0A;
    
    uint8_t currentVolume = 20;
    uint8_t currentEq     = 0;
    uint8_t currentLoop   = 2;
    
    // static const uint8_t MP3_CMD_VOL_GET = 0x43; // FIXME
    // static const uint8_t MP3_CMD_EQ_GET = 0x44; // FIXME
    // static const uint8_t MP3_CMD_LOOP_GET = 0x45; // FIXME
    // static const uint8_t MP3_CMD_VER_GET = 0x46; // FIXME
    
    // static const uint8_t MP3_CMD_COUNT_SD  = 0x0C;
    // static const uint8_t MP3_CMD_COUNT_MEM = 0x0C; 
    static const uint8_t MP3_CMD_COUNT_FILES     = 0x0C; // FIXME - No Difference for 8400?
    static const uint8_t MP3_CMD_COUNT_IN_FOLDER = 0x12; // FIXME - Implement
    
    static const uint8_t MP3_CMD_COUNT_FOLDERS        = 0x53;    
    // static const uint8_t MP3_CMD_CURRENT_FILE_IDX_SD  = 0x0D;
    // static const uint8_t MP3_CMD_CURRENT_FILE_IDX_MEM = 0x0D; 
    static const uint8_t MP3_CMD_CURRENT_FILE_IDX         = 0x0D; // FIXME - No Difference for 8400?   
    static const uint8_t MP3_CMD_FIRST_FILE_IN_FOLDER_IDX = 0x11; // FIXME - Implement
    
    
    
    // static const uint8_t MP3_CMD_CURRENT_FILE_POS_SEC = 0x50;
    // static const uint8_t MP3_CMD_CURRENT_FILE_LEN_SEC = 0x51;
    static const uint8_t MP3_CMD_CURRENT_FILE_LEN = 0x24;
    static const uint8_t MP3_CMD_CURRENT_FILE_NAME = 0x1E;
    
    
};

#endif