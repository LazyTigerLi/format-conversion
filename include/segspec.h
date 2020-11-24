/* Header for the segment by segment spectrum for noise cross correlation
 * History:
 * 1. init by wangwt to speed up CC on huge dataset.
 * 2. add cmpaz and cmpinc by wangwt@20160701
 *
 * last update wangwt@20160701 
 * */
typedef struct segspec_s {
    /* type = 1  single channel normalization and band whiten
     * type = 3  3com uniform   normalization and band whiten */
    int type;    
    /* station NSLC ID null terminated */
    char netwk[8];
    char stanm[8];
    char locid[8];
    char chnnm[8];
    float stlo;
    float stla;
    /* add cmpaz and cmpinc wangwt@20160701 */
    float cmpaz;
    float cmpinc;
    /* segment info */
    float winlen;
    float laglen;
    int   nseg;
    /* FFT info  */
    int   nspec;    /* use fftr() number of complex eg 2*nspec float */
    float df;
    float dt;
    /* ut epoch of time of 1st point */
    long long int utepoch;
} SEGSPEC;
