#include <stdio.h>
#include "src/database.h"
#include "src/server.h"

int main() {
    printf("=== SmartCampus Score System - Backend C (Final 2025) ===\n");
    printf("Initializing database...\n");
    init_database();
    printf("Starting server on http://localhost:8080\n");
    start_server("8080");
    return 0;
}