#include "protocol.h"

int main()
{
    int result = report_stdout(REPORT_INTERVAL_20MS, CONTROL_ENABLED);
    return result;
}
