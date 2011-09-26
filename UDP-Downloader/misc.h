#ifndef MISC_H
#define	MISC_H

char * prepare_message(int start, int length);

int timeval_subtract (struct timeval * result, struct timeval * x, 
                      struct timeval * y);

double printable_timeval(struct timeval tv);

#endif	/* MISC_H */

