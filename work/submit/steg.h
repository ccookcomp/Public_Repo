#ifndef STEG_H_
#define STEG_H_

#include <stddef.h>

/** Steg'd images must have this magic string stored before the message */
#define STEG_MAGIC "stg"
#define magic_number 99999

typedef enum {
  STEG_OK,       /** everything ok */
  STEG_ARG,      /** invalid Steg object */
  STEG_BAD_MAGIC,/** incorrect STEG_MAGIC in input image */
  STEG_BAD_MSG,  /** a bad message was decoded */
  STEG_FORMAT,   /** image format error */
  STEG_TOO_BIG,  /** message too big to be hidden in given image */
} StegStatus;


typedef unsigned char Byte;

/** Forward declaration of struct StegFns */
typedef struct StegFns StegFns;

/** A Steg object always has a fns pointer to StegFns.
 *  It will also contain data members which will be provided by
 *  the Steg implementation.
 */
typedef struct {
  const StegFns *fns;
} Steg;

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
StegStatus new_steg(const char *id, const Byte bytes[], int nBytes,
                    Steg **newStegP);

/** Return a string describing status */
const char *steg_status(StegStatus status);

struct StegFns {

  /** Set *idP to the id associated with this Steg object.
   *  The return'd string in *idP is valid as long as this
   *  steg object is not freed.
   *
   *  Return Values:
   *
   *    STEG_OK:        Everything ok, *idP contains id.
   *    STEG_ARG:       This Steg object is invalid.
   */
  StegStatus (*id)(const Steg *this, const char **idP);

  /** Set *nBytesP to the total number of bytes (including meta-info)
   *  occupied by the image corresponding to this Steg object.
   *
   *  Return Values:
   *
   *    STEG_OK:        Everything ok, *nBytesP contains total # bytes.
   *    STEG_ARG:       This Steg object is invalid.
   */
  StegStatus (*n_bytes)(const Steg *this, size_t *nBytesP);

  /** Set *maxMsgSizeP to the maximum client message size (including
   *  the NUL-terminator) which can be hidden in the image
   *  corresponding to this Steg object.
   *
   *  Return Values:
   *
   *    STEG_OK:        Everything ok, *maxMsgSizeP contains the max size.
   *    STEG_ARG:       This Steg object is invalid.
   */
  StegStatus (*max_msg_size)(const Steg *this, size_t *maxMsgSizeP);

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
   *        image in this Steg object except that the LSB of each
   *        pixel byte will contain the bits of magicMsg.
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
  StegStatus (*hide)(const Steg *this, const char *msg, Byte ppmOut[]);

  /** Set the contents of msg to the message hidden in this Steg
   *  object using the method specified by hide().  The caller must
   *  set up msg[] with a size of at least max_msg_size().
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
  StegStatus (*unhide)(const Steg *this, char msg[]);

  /** Free all resources occupied by this Steg object.  Subsequent uses
   *  of this Steg object should return a STEG_ARG error status.
   *
   *  Return Values:
   *
   *    STEG_OK:        Everything ok, msg successfully retrieved.
   *    STEG_ARG:       This Steg object is invalid.
   */
  StegStatus (*free)(Steg *this);

};


#endif //ifndef STEG_H_
