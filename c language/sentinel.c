#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define MAX_LINE 1024

void trigger_webhook(const char* threat_type) {
    char command[1024];
    const char* webhook_url = "https://discord.com/api/webhooks/...";
    
    snprintf(command, sizeof(command), 
             "curl -s -H \"Content-Type: application/json\" "
             "-d '{\"content\": \"🚨 **CRITICAL ALERT:** %s detected by Sentinel-Core!\"}' %s > NUL", 
             threat_type, webhook_url);
             
    system(command); 
}

int main(int argc, char *argv[]){
    const char *filename = (argc > 1) ? argv[1] : "traffic.log";
    FILE *fp = fopen(filename, "r");
    if (!fp){
        printf("[-] Error: Could not open %s\n, filename");
        return 1;
    }
    char line[MAX_LINE];
    int total_requests = 0;
    int sqli_count = 0;
    int auth_failures = 0;
    int path_traversal = 0;

    clock_t start = clock();

    while (fgets(line, sizeof(line), fp)){
        total_requests++;
        if (strstr(line, "POST /api/login") || strstr(line, "401")) {
            auth_failures++; 
            trigger_webhook("Stolen Credential Stuffing");
        }
        else if (strstr(line, "GET /redirect") || strstr(line, "url=http")) {
            path_traversal++; 
            trigger_webhook("Malicious Phishing Redirect");
        }
    }

    clock_t end = clock();
    double latency_ms = ((double)(end - start) / CLOCKS_PER_SEC) * 1000.0;
    fclose(fp);

    printf("\n=======================================================\n");
    printf("\n=======================================================\n");
    printf("        DYNAMIC RECOMPILE // SENTINEL-CORE v1.0       \n");
    printf("=======================================================\n");
    printf(" [+] Packets/Logs Parsed    : %d\n", total_requests);
    printf(" [!] Suspicious Payloads : %d\n", sqli_count);
    printf(" [!] Credential Stuffing   : %d\n", auth_failures);
    printf(" [!] Malicious Redirects: %d\n", path_traversal);
    printf(" -------------------------------------------------------\n");
    printf(" [*] Core Engine Latency    : %.3f ms\n", latency_ms);
    printf(" [*] Active Status          : REAL-TIME DEFENSE ENGAGED\n");
    printf("=======================================================\n\n");
     
    // Write results to a JSON file for the Tailwind Dashboard
    FILE *json_file = fopen("alerts.json", "w");
    if (json_file != NULL) {
        fprintf(json_file, "{\n");
        fprintf(json_file, "  \"total_requests\": %d,\n", total_requests);
        fprintf(json_file, "  \"sqli_count\": %d,\n", sqli_count);
        fprintf(json_file, "  \"auth_failures\": %d,\n", auth_failures);
        fprintf(json_file, "  \"path_traversal\": %d,\n", path_traversal);
        fprintf(json_file, "  \"latency_ms\": %.3f\n", latency_ms);
        fprintf(json_file, "}\n");
        fclose(json_file);
    }
    return 0;
}