#ifndef _REQUESTS_
#define _REQUESTS_

char *compute_get_request(const char *host,
	                  const char *url,
	                  const char *url_params,
	                  const char *cookies,
	                  const char *authorization);

char *compute_post_request(const char *host,
	                   const char *url,
	                   const char *method,
	                   const char *form_data,
	                   const char *cookies,
	                   const char *authorization);

char *compute_delete_request(const char *host,
	                   const char *url,
	                   const char *cookies,
	                   const char *authorization);

#endif