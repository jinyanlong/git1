// #include <stdio.h>
// #include <curl/curl.h>

// int main(void)
// {
//   CURL *curl;
//   CURLcode res;

//   /* http://curl.haxx.se/libcurl/c/curl_easy_init.html */
//   curl = curl_easy_init();
//   if(curl) {
//     /* http://curl.haxx.se/libcurl/c/curl_easy_setopt.html#CURLOPTURL */
//     curl_easy_setopt(curl, CURLOPT_URL, "http://www.quectel.com/");
//     /* http://curl.haxx.se/libcurl/c/curl_easy_perform.html */
//     res = curl_easy_perform(curl);

//     if(CURLE_OK == res) {
//       char *ct;
//       /* ask for the content-type */
//       /* http://curl.haxx.se/libcurl/c/curl_easy_getinfo.html */
//       res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);

//       if((CURLE_OK == res) && ct)
//         printf("We received Content-Type: %s\n", ct);
//     }

//     /* always cleanup */
//     /* http://curl.haxx.se/libcurl/c/curl_easy_cleanup.html */
//     curl_easy_cleanup(curl);
//   }
//   return 0;
// }


// ###################@@1-jyl-test###########
#include <stdio.h>
#include <curl/curl.h>

#include <json.h>
#include <string.h>
#include <fcntl.h>
int creatjson(char *json_str)
{
    int i=0;
    struct json_object *obj = json_object_new_object();  
    struct json_object *arr_month_grade = json_object_new_array();
    struct json_object *obj_int;

    for(i=1; i<13; i++){  
        obj_int = json_object_new_int(i);  
        json_object_array_add(arr_month_grade, obj_int);      
    }  
  
    json_object_object_add(obj, "method", json_object_new_string("sendRecords"));  
    json_object_object_add(obj, "id", json_object_new_string("123"));        
    json_object_object_add(obj, "params", arr_month_grade);        
      
    const char *str = json_object_to_json_string(obj);
    memcpy(json_str,str,300);
 
    json_object_put(obj);
    
    return 0;  
} 

void postUrl(void)
{
    CURL *curl;
    CURLcode res;

    char json_str[300];
    creatjson(json_str);

    curl = curl_easy_init();
    if (curl)
    {
        // curl_easy_setopt(curl, CURLOPT_COOKIEFILE, "/tmp/cookie.txt"); // 指定cookie文件
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str);    // 指定post内容
        //curl_easy_setopt(curl, CURLOPT_PROXY, "10.99.60.201:8080");
        curl_easy_setopt(curl, CURLOPT_URL, "http://server.yz-online.com:9080/DeviceService/public/smd/tagIO.smd");   // 指定url
        // curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    // fclose(fp);
    return ;
}
int main(void)
{
    // getUrl("/tmp/get.html");
    while(1){
        postUrl();
        sleep(1);
    }
}
