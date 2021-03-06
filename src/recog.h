/*====================================================================*
 -  Copyright (C) 2001 Leptonica.  All rights reserved.
 -
 -  Redistribution and use in source and binary forms, with or without
 -  modification, are permitted provided that the following conditions
 -  are met:
 -  1. Redistributions of source code must retain the above copyright
 -     notice, this list of conditions and the following disclaimer.
 -  2. Redistributions in binary form must reproduce the above
 -     copyright notice, this list of conditions and the following
 -     disclaimer in the documentation and/or other materials
 -     provided with the distribution.
 -
 -  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 -  ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 -  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 -  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL ANY
 -  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 -  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 -  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 -  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 -  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 -  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 -  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *====================================================================*/

#ifndef  LEPTONICA_RECOG_H
#define  LEPTONICA_RECOG_H

/*!
 * \file recog.h
 *
 * <pre>
 *     This is a simple utility for training and recognizing individual
 *     machine-printed text characters.  It is designed to be adapted
 *     to a particular set of character images; e.g., from a book.
 *
 *     There are two methods of training the recognizer.  In the most
 *     simple, a set of bitmaps has been labeled by some means, such
 *     a generic OCR program.  This is input to a recog creator either
 *     one at a time or in a pixa.  If in a pixa, the labeling text
 *     string must be embedded in the text field of each pix.
 *
 *     If labeled data is not available, we start with a bootstrap
 *     recognizer (BSR) that has labeled data from a variety of sources.
 *     These images are scaled, typically to a fixed height, and then
 *     fed similarly scaled unlabeled images from the source (e.g., book),
 *     and the BSR attempts to identify them.  All images that have a high
 *     enough correlation score with one of the templates in the BSR
 *     are emitted.  The resulting pixa has the unscaled and labeled
 *     templates from the source; this is the generator for a book adapted
 *     recognizer (BAR).
 *
 *     The pixa should always be thought of as the primary structure.
 *     It is the generator for the recog, because a recog is built
 *     from a pixa of unscaled images or unscaled normalized outlines.
 *
 *     New image templates can be added to a recog as long as it is
 *     in training mode.  Once training is finished, to add templates
 *     it is necessary to extract the generating pixa, add templates
 *     to that pixa, and make a new recog.
 *
 *     We do not join two recog.  Instead, we simply join their
 *     generating pixa, and make a recog from that.
 *
 *     To remove outliers from a pixa of labeled pix, make a recog,
 *     determine the outliers, and generate a new pixa with the
 *     outliers removed.  The outliers are determined by building
 *     special templates for each character set that are scaled averages
 *     of the individual templates.  Then a correlation score is found
 *     between each template and the averaged templates.  If a template
 *     is better correlated with a set different from its own label,
 *     it is deemed an "outlier" and removed from the generating pixa.
 *     Scaled averaging is only performed for determining outliers,
 *     not for identifying unlabeled characters.
 *
 *     Once a BAR has been made, unlabeled input images are identified
 *     by finding the individual template in the BAR with highest
 *     correlation.  The input images and images in the BAR can be
 *     represented in two ways:
 *      (1) as scanned, binarized to 1 bpp
 *      (2) as a width-normalized outline formed by thinning to a
 *          skeleton and then dilating by a fixed amount.
 *
 *     The recog can be serialized to file and read back.  The serialized
 *     version holds all the bitmaps used for training, plus, for arbitrary
 *     character sets, the UTF8 representation and the lookup table
 *     mapping from the character representation to index.
 *
 *     Why do we not use averaging for recognition?  Letterforms can
 *     take on significantly different shapes (eg., the letters 'a' and 'g'),
 *     and it makes no sense to average these.  The previous version of
 *     this utility allowed multiple recognizers to exist, but this
 *     is an unnecessary complication if recognition is done on all
 *     samples (instead of averages).
 * </pre>
 */

#define  RECOG_VERSION_NUMBER      2

struct L_Recog {
    l_int32        scalew;       /*!< scale all examples to this width;      */
                                 /*!< use 0 prevent horizontal scaling       */
    l_int32        scaleh;       /*!< scale all examples to this height;     */
                                 /*!< use 0 prevent vertical scaling         */
    l_int32        templ_type;   /*!< template type: either the image or a   */
                                 /*!< normalized outline (L_TYPE_IMAGE or    */
                                 /*!< L_TYPE_OUTLINE)                        */
    l_int32        templ_use;    /*!< template use: use either the average   */
                                 /*!< or all temmplates (L_USE_AVERAGE or    */
                                 /*!< L_USE_ALL)                             */
    l_int32        maxarraysize; /*!< initialize container arrays to this    */
    l_int32        setsize;      /*!< size of character set                  */
    l_int32        threshold;    /*!< for binarizing if depth > 1            */
    l_int32        maxyshift;    /*!< vertical jiggle on nominal centroid    */
                                 /*!< alignment; typically 0 or 1            */
    l_int32        charset_type; /*!< one of L_ARABIC_NUMERALS, etc.         */
    l_int32        charset_size; /*!< expected number of classes in charset  */
    char          *bootdir;      /*!< dir with bootstrap pixa charsets       */
    char          *bootpattern;  /*!< file pattern: bootstrap pixa charsets  */
    char          *bootpath;     /*!< path for single bootstrap pixa charset */
    l_int32        boot_iters;   /*!< num of 2x2 erosion iters on boot pixa  */
    l_int32        min_nopad;    /*!< min number of samples without padding  */
    l_int32        max_afterpad; /*!< max number of samples after padding    */
    l_int32        min_samples;  /*!< min num of total samples; else use boot */
    l_int32        num_samples;  /*!< number of training samples             */
    l_int32        minwidth_u;   /*!< min width averaged unscaled templates  */
    l_int32        maxwidth_u;   /*!< max width averaged unscaled templates  */
    l_int32        minheight_u;  /*!< min height averaged unscaled templates */
    l_int32        maxheight_u;  /*!< max height averaged unscaled templates */
    l_int32        minwidth;     /*!< min width averaged scaled templates    */
    l_int32        maxwidth;     /*!< max width averaged scaled templates    */
    l_int32        ave_done;     /*!< set to 1 when averaged bitmaps are made */
    l_int32        train_done;   /*!< set to 1 when training is complete or  */
                                 /*!< identification has started             */
    l_int32        min_splitw;   /*!< min component width kept in splitting  */
    l_int32        min_splith;   /*!< min component height kept in splitting */
    l_int32        max_splith;   /*!< max component height kept in splitting */
    struct Sarray *sa_text;      /*!< text array for arbitrary char set      */
    struct L_Dna  *dna_tochar;   /*!< index-to-char lut for arbitrary charset */
    l_int32       *centtab;      /*!< table for finding centroids            */
    l_int32       *sumtab;       /*!< table for finding pixel sums           */
    struct Pixaa  *pixaa_u;      /*!< all unscaled bitmaps for each class    */
    struct Pixa   *pixa_u;       /*!< averaged unscaled bitmaps per class    */
    struct Ptaa   *ptaa_u;       /*!< centroids of all unscaled bitmaps      */
    struct Pta    *pta_u;        /*!< centroids of unscaled averaged bitmaps */
    struct Numaa  *naasum_u;     /*!< area of all unscaled bitmap examples   */
    struct Numa   *nasum_u;      /*!< area of unscaled averaged bitmaps      */
    struct Pixaa  *pixaa;        /*!< all bitmap examples for each class     */
    struct Pixa   *pixa;         /*!< averaged bitmaps for each class        */
    struct Ptaa   *ptaa;         /*!< centroids of all bitmap examples       */
    struct Pta    *pta;          /*!< centroids of averaged bitmaps          */
    struct Numaa  *naasum;       /*!< area of all bitmap examples            */
    struct Numa   *nasum;        /*!< area of averaged bitmaps               */
    struct Pixa   *pixa_tr;      /*!< all input training images              */
    struct Pixa   *pixadb_ave;   /*!< unscaled and scaled averaged bitmaps   */
    struct Pixa   *pixa_id;      /*!< input images for identifying           */
    struct Pix    *pixdb_ave;    /*!< debug: best match of input against ave. */
    struct Pix    *pixdb_range;  /*!< debug: best matches within range       */
    struct Pixa   *pixadb_boot;  /*!< debug: bootstrap training results      */
    struct Pixa   *pixadb_split; /*!< debug: splitting results               */
    struct L_Bmf  *bmf;          /*!< bmf fonts                              */
    l_int32        bmf_size;     /*!< font size of bmf; default is 6 pt      */
    struct L_Rdid *did;          /*!< temp data used for image decoding      */
    struct L_Rch  *rch;          /*!< temp data used for holding best char   */
    struct L_Rcha *rcha;         /*!< temp data used for array of best chars */
};
typedef struct L_Recog L_RECOG;

/*!
 *  Data returned from correlation matching on a single character
 */
struct L_Rch {
    l_int32        index;      /*!< index of best template                   */
    l_float32      score;      /*!< correlation score of best template       */
    char          *text;       /*!< character string of best template        */
    l_int32        sample;     /*!< index of best sample (within the best    */
                               /*!< template class, if all samples are used) */
    l_int32        xloc;       /*!< x-location of template (delx + shiftx)   */
    l_int32        yloc;       /*!< y-location of template (dely + shifty)   */
    l_int32        width;      /*!< width of best template                   */
};
typedef struct L_Rch L_RCH;

/*!
 *  Data returned from correlation matching on an array of characters
 */
struct L_Rcha {
    struct Numa   *naindex;    /*!< indices of best templates                */
    struct Numa   *nascore;    /*!< correlation scores of best templates     */
    struct Sarray *satext;     /*!< character strings of best templates      */
    struct Numa   *nasample;   /*!< indices of best samples                  */
    struct Numa   *naxloc;     /*!< x-locations of templates (delx + shiftx) */
    struct Numa   *nayloc;     /*!< y-locations of templates (dely + shifty) */
    struct Numa   *nawidth;    /*!< widths of best templates                 */
};
typedef struct L_Rcha L_RCHA;

/*!
 *  Data used for decoding a line of characters.
 */
struct L_Rdid {
    struct Pix    *pixs;         /*!< clone of pix to be decoded             */
    l_int32      **counta;       /*!< count array for each averaged template */
    l_int32      **delya;        /*!< best y-shift array per average template */
    l_int32        narray;       /*!< number of averaged templates           */
    l_int32        size;         /*!< size of count array (width of pixs)    */
    l_int32       *setwidth;     /*!< setwidths for each template            */
    struct Numa   *nasum;        /*!< pixel count in pixs by column          */
    struct Numa   *namoment;     /*!< first moment of pixels in pixs by cols */
    l_int32        fullarrays;   /*!< 1 if full arrays are made; 0 otherwise */
    l_float32     *beta;         /*!< channel coeffs for template fg term    */
    l_float32     *gamma;        /*!< channel coeffs for bit-and term        */
    l_float32     *trellisscore; /*!< score on trellis                       */
    l_int32       *trellistempl; /*!< template on trellis (for backtrack)    */
    struct Numa   *natempl;      /*!< indices of best path templates         */
    struct Numa   *naxloc;       /*!< x locations of best path templates     */
    struct Numa   *nadely;       /*!< y locations of best path templates     */
    struct Numa   *nawidth;      /*!< widths of best path templates          */
    struct Numa   *nascore;      /*!< correlation scores: best path templates */
    struct Numa   *natempl_r;    /*!< indices of best rescored templates     */
    struct Numa   *naxloc_r;     /*!< x locations of best rescoredtemplates  */
    struct Numa   *nadely_r;     /*!< y locations of best rescoredtemplates  */
    struct Numa   *nawidth_r;    /*!< widths of best rescoredtemplates       */
    struct Numa   *nascore_r;    /*!< correlation scores: rescored templates */
};
typedef struct L_Rdid L_RDID;


/*-------------------------------------------------------------------------*
 *                     Flags for template scaling                          *
 *-------------------------------------------------------------------------*/
/*! Flags for template scaling */
enum {
    L_SELECT_UNSCALED = 0,     /*!< select the unscaled bitmaps            */
    L_SELECT_SCALED = 1,       /*!< select the scaled bitmaps              */
    L_SELECT_BOTH = 2          /*!< select both unscaled and scaled        */
};

/*-------------------------------------------------------------------------*
 *          Flags for selecting between image and outline templates        *
 *-------------------------------------------------------------------------*/
/*! Flags for selecting image or outline templates: recog->templ_type */
enum {
    L_TYPE_IMAGE = 0,         /*!< match scanned images                    */
    L_TYPE_OUTLINE = 1        /*!< match thickened outlines                */
};

/*-------------------------------------------------------------------------*
 *      Flags for selecting between using average and all templates        *
 *-------------------------------------------------------------------------*/
/*! Flags for selecting average or all templates: recog->templ_use */
enum {
    L_USE_ALL = 0,            /*!< use all templates; default              */
    L_USE_AVERAGE = 1         /*!< use average templates; outliers only    */
};

/*-------------------------------------------------------------------------*
 *             Flags for describing limited character sets                 *
 *-------------------------------------------------------------------------*/
/*! Flags for describing limited character sets */
enum {
    L_UNKNOWN = 0,           /*!< character set type is not specified      */
    L_ARABIC_NUMERALS = 1,   /*!< 10 digits                                */
    L_LC_ROMAN_NUMERALS = 2, /*!< 7 lower-case letters (i,v,x,l,c,d,m)     */
    L_UC_ROMAN_NUMERALS = 3, /*!< 7 upper-case letters (I,V,X,L,C,D,M)     */
    L_LC_ALPHA = 4,          /*!< 26 lower-case letters                    */
    L_UC_ALPHA = 5           /*!< 26 upper-case letters                    */
};

#endif  /* LEPTONICA_RECOG_H */
