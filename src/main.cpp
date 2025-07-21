#include <iostream>

#include "NFCHandler.h"

NFCHandler::Conf config = {.debug = true, .read_data = false};
NFCHandler nfc_handler(config);

int main()
{
    std::cout << "Let's read NFC tags!" << std::endl;

    nfc_handler.init();

    nfc_handler.tic();

    std::cout << "Exiting program." << std::endl;

    return 0;
}
