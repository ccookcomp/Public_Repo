//program includes
#include "steg.h"

//uncomment following line to activate TRACE(...) calls in code
//#define DO_TRACE 1

//cs551 includes
#include "memalloc.h"
#include "trace.h"

//system includes
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

//Store this within a Steg structure to mark it as valid
#define STEG_VALID "STG"

/** A StegImpl contains a Steg plus specific implementation data */
typedef struct {
  Steg;     //non-standard; depends on -f-ms-extensions
  const char *id; 
  int totalBytes;
  int magicNum;//Num = 99999 or magic_number if steg is valid
  int start; //Index of the first byte after the header
  Byte *bytes1;
  int freed; //Used to tell if the steg has been freed. 1 for freed, 0 for not
} StegImpl;

//static StegImpl *stegI;
/************************** StegFns Functions **************************/


/** Set *idP to the id associated with this Steg object.
 *  Return Values:
 *
 *    STEG_OK:        Everything ok, *idP contains id.
 *    STEG_ARG:       This Steg object is invalid.
 */
static StegStatus
id(const Steg *this, const char **idP)
{
  StegImpl *steg= (StegImpl*)this;
  if(steg->magicNum != magic_number) return STEG_ARG;
  *idP=steg->id;
  return STEG_OK;
}


/** Set *nBytesP to the total number of bytes (including meta-info)
 *  occupied by the image corresponding to this Steg object.
 *
 *  Return Values:
 *
 *    STEG_OK:        Everything ok, *nBytesP contains total # bytes.
 *    STEG_ARG:       This Steg object is invalid.
 */
static StegStatus
n_bytes(const Steg *this, size_t *nBytesP)
{
  StegImpl *steg= (StegImpl*)this;
  if(steg->magicNum != magic_number) return STEG_ARG;
  *nBytesP=steg->totalBytes;
  return STEG_OK;
}

/** Set *maxMsgSizeP to the maximum message size (including the
 *  NUL-terminator) which can be hidden in the image corresponding
 *  to this Steg object.
 *
 *  Return Values:
 *
 *    STEG_OK:        Everything ok, *maxMsgSizeP contains the max size.
 *    STEG_ARG:       This Steg object is invalid.
 */
static StegStatus
max_msg_size(const Steg *this, size_t *maxMsgSizeP)
{
  StegImpl *steg= (StegImpl*)this;
  if(steg->magicNum != magic_number) return STEG_ARG;
  *maxMsgSizeP=(size_t)((steg->totalBytes - steg->start)/8);
	//8 is used for the bytes needed for one ascii character
  return STEG_OK;
}

/** Hide NUL-terminated message msg in PPM image specified by this
 *  Steg object storing the result in PPM image ppmOut[].  The
 *  caller must set up ppmOut[] with a size of at least n_bytes().
 *
 *  The ppmOut image will be formed from the image contained in this
 *  Steg object and msg as follows.
 *
 *    1.  The meta-info (header, comments, resolution, color-depth)
 *        for ppmOut is set to that of the PPM image contained in
 *        this Steg object.
 *
 *    2.  A magicMsg is formed as the concatenation of STEG_MAGIC and
 *        msg.
 *
 *    3.  The bits of magicMsg (including the terminating
 *        NUL-character) are unpacked (MSB-first) into the LSB of
 *        successive pixel bytes of the ppmOut image.  Note that the
 *        pixel bytes of ppmOut should be identical to those of the
 *        image in this Steg object except that the LSB of each pixel
 *        byte will contain the bits of magicMsg.
 *
 *  Return Values:
 *
 *    STEG_OK:        Everything ok, msg successfully hidden in ppmOut.
 *    STEG_ARG:       This Steg object is invalid.
 *    STEG_TOO_BIG:   n_bytes() is not large enough to allow hiding magicMsg.
 *    STEG_BAD_MAGIC: The image contained in this Steg object may already
 *                    contain a hidden message; detected by seeing
 *                    this Steg object's underlying image pixel bytes
 *                    starting with a hidden STEG_MAGIC string.
 *
 */
 static StegStatus
 hide(const Steg *this, const char *msg, Byte ppmOut[])
 {
   StegImpl *steg= (StegImpl*)this;
   if(steg->magicNum != magic_number) return STEG_ARG;
	int size=0;
	while(msg[size]!='\0'){
		size++;
	}
   if((steg->totalBytes-steg->start)/8 < size) return STEG_TOO_BIG;
   int i=0;
   int count;
   unsigned int ascii;
   unsigned int tascii;
   char valid[3] = {'S','T','G'};
   char msg1[3]; //3 to get the STG
   int msgI=0;
   unsigned int final=0;
   int evenOrOdd=0;
   int totalBytesUsed=steg->start;
   int byte=steg->start;
//Check for STEG_BAD_MAGIC
   for(i=0;i<3;i++){
	//8 is used for the bytes needed for one ascii character
		for(count=0;count<8;count++){
			evenOrOdd= (unsigned int)steg->bytes1[byte];
			byte++;
			evenOrOdd=evenOrOdd % 2; //2 to determine if it is even or odd
			final = final<<1;
			final+=evenOrOdd;
			
		}
		msg1[msgI]=(char)final;
		msgI++;
		final=0;
	}
   if(msg1[0]=='S' && msg1[1]=='T' && msg1[2]=='G'){
		steg->magicNum=0;
   		return STEG_BAD_MAGIC;
   }
   i=0;
//Hides the message with STG in front of the message
   while (i<steg->start){
		ppmOut[i]=steg->bytes1[i];
		i++;
	}
	for(i=0; i<=size+3; i++){
		if(i<3){
			ascii=(int)valid[i];
		}else if(i==size+3){
			ascii=(unsigned int)'\0';
		}
		else{
			ascii=(unsigned int)msg[i-3];
		}
		//8 is used for the bytes needed for one ascii character
		for(count=(i+1)*8+steg->start-1;count>=(i)*8+steg->start;count--){
			totalBytesUsed++;
			tascii=ascii%2;
			if(steg->bytes1[count]%2 != tascii){
				if(steg->bytes1[count]%2 == 0){
					ppmOut[count]=steg->bytes1[count]+1;
				}
				else{
					ppmOut[count]=steg->bytes1[count]-1;
					}
			}else{ 
				ppmOut[count]=steg->bytes1[count];
			}
			ascii=ascii>>1;
		}
		ascii=0;
	}
	for(i=totalBytesUsed;i<steg->totalBytes;i++){
		ppmOut[i]=steg->bytes1[i];
	}
   return STEG_OK;
 }


/** Set the contents of msg to the message hidden in this Steg object
 *  using the method specified by hide().  The caller must set up
 *  msg[] with a size of at least max_msg_size(this).
 *
 *  Return Values:
 *
 *    STEG_OK:        Everything ok, msg successfully retrieved.
 *    STEG_ARG:       This Steg object is invalid.
 *    STEG_BAD_MAGIC: The image contained in this Steg object does not
 *                    contain a hidden message; detected by not
 *                    seeing this Steg object's underlying image pixel
 *                    bytes starting with a hidden STEG_MAGIC
 *                    string.
 *    STEG_BAD_MSG:   A bad message was decoded (the NUL-terminator
 *                    was not found).
 *
 */
static StegStatus
unhide(const Steg *this, char msg[])
{
  StegImpl *steg= (StegImpl*)this;
  if(steg->magicNum != magic_number) return STEG_ARG;
  int start = steg->start;
  int i=start;
  int totalLetters=0;
  int msgI=0;
  int count=0;
  int stop=0;
  unsigned int evenOrOdd;
  unsigned int final=0;
  char msg1[((steg->totalBytes - steg->start)/8)];

  while(stop==0){
	//8 is used for the bytes needed for one ascii character
		for(count=0;count<8;count++){
			if(i==steg->totalBytes-1){//Checks for STEG_BAD_MSG
				steg->magicNum=0;
				return STEG_BAD_MSG;
			}
//Gets the last byte and then adds it to int final which is bit shifted to the
//left one.
			evenOrOdd= (int)steg->bytes1[i];
			i++;
			evenOrOdd=evenOrOdd % 2;
			final = final<<1;
			final+=evenOrOdd;
		}
		msg1[msgI]=(char)final;//Adds the 8 bit char value to the msg Array
		if(msg1[msgI]=='\0')stop=1;
		totalLetters++;
		msgI++;
		final=0;
	}
	if(msg1[0]!='S' && msg1[1]!='T' && msg1[2]!='G'){
		steg->magicNum=0;
   		return STEG_BAD_MAGIC; //Checks for STEG_BAD_MAGIC
   }
	memcpy(msg,&msg1[3],totalLetters-3);//3 to cut out the STG in the beginning
	msg[totalLetters-3]='\0';
  return STEG_OK;
}

/** Free all resources occupied by this Steg object.  Subsequent uses
 *  of this Steg object should return a STEG_ARG error status.
 */
static StegStatus
free_steg(Steg *this)
{
  StegImpl *steg= (StegImpl*)this;
  if(steg->freed==1){
	return STEG_ARG;
	}
  free(steg->bytes1);
  free(steg);
  steg->freed=1;
  steg->magicNum=0;
  return STEG_OK;
}

/** Shared by all Steg objects by being pointed to by initial fns member. */
static StegFns fns = {
  .id = id,
  .n_bytes = n_bytes,
  .max_msg_size = max_msg_size,
  .hide = hide,
  .unhide = unhide,
  .free = free_steg,
};

//Used to se the fns to the steg
StegFns *getStegFns(void){
	return &fns;
}
/*************************** Extern Functions **************************/

//These functions below set the values of steg Impl based on what is passed
//Into new_steg

void setStartIndex(const Steg *this, int i){
	StegImpl *steg= (StegImpl*)this;
	steg->start=i;
}
void setArray(const Steg *this, const Byte Arr[]){
	StegImpl *steg= (StegImpl*)this;
	steg->bytes1=malloc(steg->totalBytes);
	int i=0;
	for(i=0;i<steg->totalBytes;i++){
		steg->bytes1[i]=Arr[i];
	}
}

void setNBytes(const Steg *this, int nBytes){
	StegImpl *steg= (StegImpl*)this;
	steg->totalBytes=nBytes;

}
void setId(const Steg *this,const char *id1){
	StegImpl *steg= (StegImpl*)this;
	steg->id=id1;
}

void setMagicNum(const Steg *this, int mag){
	StegImpl *steg= (StegImpl*)this;
	steg->magicNum=mag;
}
void setFreed(const Steg *this){
	StegImpl *steg= (StegImpl*)this;
	steg->freed=0;
}
/** Return a pointer to a newly created Steg object in *newStegP
 *  created from bytes[nBytes] having the specified id.  This routine
 *  makes its own copy of *id and bytes[] so they need not remain valid
 *  after this call returns.  When the return'd object is no longer
 *  needed it should be called by calling it's free() method.
 *
 *  The program will be terminated with an error message if there is
 *  insufficient system memory to allocate this object.
 *
 *  It is assumed that bytes[] contains all the bytes of a P6 PPM
 *  image, including meta-information.  Specifically, bytes[] should
 *  be in the following format:
 *
 *     1.  A 2-byte "magic number", specifying the format.  Only legal values
 *         for this project are "P6" (in ASCII).
 *     2.  One of more whitespace characters as defined by the isspace()
 *         function from <ctype.h>
 *     3.  A positive ASCII integer giving the width X of the image in pixels.
 *     4.  Whitespace as above.
 *     5.  A positive ASCII integer giving the height Y of the image in pixels.
 *     6.  Whitespace as above.
 *     7.  A positive ASCII integer giving the maximum color value of each
 *         pixel.  For this project, this value should always be 255.
 *     8.  A single whitespace character.
 *     9.  The data for the pixels in the image.  Since each pixel is
 *         represented by 3 RGB color values, the pixel data must have
 *         exactly 3*X*Y bytes.
 *
 *  Return Values:
 *
 *    STEG_OK:         Everything ok, the new Steg object was
 *                     successfully created and is pointed to
 *                     by *newStegP.
 *    STEG_FORMAT:     bytes[] does not conform to the format
 *                     documented above.
 */
StegStatus
new_steg(const char *id1, const Byte bytes[], int nBytes,
         Steg **newStegP)
{
  *newStegP= malloc(sizeof(StegImpl));
  (*newStegP)->fns=getStegFns();
  setFreed(*newStegP);
  setId(*newStegP, id1);
  setNBytes(*newStegP,nBytes);
  setArray(*newStegP, bytes);
  int i;
  Byte mN[3];
  memcpy(mN,bytes,2);
  mN[2]='\0';
  if(mN[0] != 'P' && mN[1] != '6'){
     return STEG_FORMAT; //Ensures "P6" starts the header
	}
  if(bytes[2]=='\n'){
	i=3;
	int start=3;
	int count=0;
	while (bytes[i]!=' '){
		if(!(isdigit(bytes[i]))){
			return STEG_FORMAT;//Ensures newline followed by an int
		}
		count++;
		i++;
	}
	Byte num1[count+1];
	memcpy(num1,&bytes[start],count);
	num1[count]='\0';
	if(bytes[i]==' '){
		i++;
		start=i;
		count=0;
		while (bytes[i]!='\n'){
			if(!(isdigit(bytes[i]))){
				return STEG_FORMAT;//Ensures whitespace followed by an int
			}
			count++;
			i++;
		}
		Byte num2[count+1];
		memcpy(num2,&bytes[start],count);
		num2[count]='\0';
		}
	if(bytes[i]=='\n'){
		i++;
		start=i;
		count=0;
		while (bytes[i]!='\n'){
			if(!(isdigit(bytes[i]))){
				return STEG_FORMAT;//Ensures newline followed by an int.
			}
			count++;
			i++;
		}
		Byte num3[count+1];
		memcpy(num3,&bytes[start],count);
		num3[count]='\0';
		}
	i++;
	setMagicNum(*newStegP,magic_number);
	setStartIndex(*newStegP,i);
	}else return STEG_FORMAT;
  
  return STEG_OK;
}



/** Return a string describing status */
const char *
steg_status(StegStatus status)
{
  switch (status) {
  case STEG_OK:
    return "ok";
  case STEG_ARG:
    return "invalid steg object";
  case STEG_BAD_MAGIC:
    return "incorrect STEG_MAGIC in input image";
  case STEG_BAD_MSG:
    return "invalid message decoded";
  case STEG_FORMAT:
    return "image format error";
  case STEG_TOO_BIG:
    return "message too big to be hidden in given image";
  default:
    return "invalid Steg status";
  }
}
