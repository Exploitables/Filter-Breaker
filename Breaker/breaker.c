#pragma warning (disable : 28159)

#include "breaker.h"

int main(int argc, char** argv)
{
	PWCHAR unicode_port_name = (PWCHAR)0;
	void* h_port = INVALID_HANDLE_VALUE;
	unsigned long long max_length = 0ULL;
	unsigned int context = 0U;
	unsigned char verbose = 0;

	unsigned long long size = 0ULL;
	unsigned long long total_requests = 0ULL;
	char* input = (char*)0;
	unsigned long bytes_returned = 0UL;
	long h_connect_result = 0L;
	long h_request_result = 0L;

	srand(GetTickCount());
	SetConsoleTitleA("Mini-Filter Breaker");

	printf("%s", PREAMBLE);

	if (!argv[1] || !argv[2] || !argv[3] || !argv[4])
	{
		printf("[*] Usage: Breaker.exe <Communication Port> <Connection Context> <Maximum Input Length> <Verbose>\n");
		return 1;
	}

	verbose = atoi(argv[4]);
	max_length = atoi(argv[3]);
	context = atoi(argv[2]);

	unicode_port_name = (PWCHAR)malloc(sizeof(WCHAR) * (strlen(argv[1]) + 1));
	if (!unicode_port_name)
	{
		printf("[-] Failed to allocate a wide-character port name.");
		return 1;
	}

	if (!MultiByteToWideChar(CP_OEMCP, 0, argv[1], -1, unicode_port_name, strlen(argv[1]) + 1))
	{
		printf("[-] Failed to create a wide-character port name. Error: %d (0x%x)", GetLastError(), GetLastError());
		return 1;
	}

	printf("[*] Communication Port: %s\n[*] Connection Context: %s\n[*] Maximum Length: %s\n\n", argv[1], argv[2], argv[3]);

	h_connect_result = FilterConnectCommunicationPort(unicode_port_name, 0, &context, sizeof(context), 0, &h_port);
	if (h_connect_result < 0 || h_port == INVALID_HANDLE_VALUE)
	{
		printf("[-] Failed to establish a filter connection.\n");
		return 1;
	}
	printf("[+] Established a filter connection. Handle Value: 0x%p\n[*] Fuzzing...\n", h_port);

	while (1)
	{
		size = rand() % max_length;
		input = (char*)malloc(size);
		if (!input)
		{
			printf("[-] Failed to allocate input buffer.\n");
			return 1;
		}

		for (int i = 0; i < size; i++)
		{
			*(char*)(input + i) = rand() % 0xFF;
		}

		h_request_result = FilterSendMessage(h_port, input, size, 0, 0, &bytes_returned);
		if (verbose)
		{
			if (h_request_result)
			{
				printf("[-] Request failed.\n");
			}
			else
			{
				printf("[+] Request succeeded.\n");
			}
		}

		free(input);

		total_requests++;
		if (!(total_requests % 100000))
		{
			printf("[*] Total requests sent: %llu (0x%p)\n", total_requests, (void*)total_requests);
		}
	}

	return 0;
}