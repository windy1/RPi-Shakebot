#ifndef SHAKESPEARE_CURL_UTILS_H
#define SHAKESPEARE_CURL_UTILS_H

#include "rest.h"
#include <curl/curl.h>

namespace sb {

    /**
     * A REST response
     */
    struct RestResponse {

        CURLcode    code;
        size_t      size;
        char        *data = NULL;

        RestResponse();

        RestResponse(const RestResponse &response);

        ~RestResponse();

        /**
         * Returns the response data as json.
         *
         * @return
         */
        json asJson();

        friend ostream& operator<<(ostream &out, const RestResponse &response);

    };

    /**
     * A cURL wrapper designed to interface with REST APIs
     */
    class RestClient : public NonAssignable {

        static const string DEFAULT_RESULT_TYPE;

        CURL            *curl           = NULL;
        curl_slist      *requestHeaders = NULL;
        bool            verbose         = true;
        RestResponse    response;

        /// performs a new request
        RestResponse* perform(string url, bool post, string resultType = DEFAULT_RESULT_TYPE);

    public:

        ~RestClient();

        CURL* getCurl() const;

        /**
         * Returns true if cURL should have verbose logging enabled.
         *
         * @return True if verbose mode enabled
         */
        bool isVerbose() const;

        /**
         * Sets whether cURL should have verbose loggin enabled.
         *
         * @param verbose True if verbose mode enabled
         */
        void setVerbose(bool verbose);

        /**
         * Adds a content header to the cURL client.
         *
         * @param header Header to add
         */
        void addHeader(string header);

        /**
         * Initializes this client and cURL.
         *
         * @return True if successful, false otherwise
         */
        bool init();

        /**
         * Performs a POST request.
         *
         * @param data Data to POST
         * @param url URL to POST to
         * @param resultType Expected content type in response
         * @return A RestResponse if successful, NULL otherwise
         */
        RestResponse* post(string data, string url, string resultType = DEFAULT_RESULT_TYPE);

        /**
         * Performs a GET request.
         *
         * @param url URL to GET
         * @param resultType Expected content type in response
         * @return A RestResponse if successful, NULL otherwise
         */
        RestResponse* get(string url, string resultType = DEFAULT_RESULT_TYPE);

    };

}

#endif //SHAKESPEARE_CURL_UTILS_H
