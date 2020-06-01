/* BLE Helper Functions
 *
 * March 7, 2020
 *
 */


// == FEED THE WATCHDOG TIMER ==
// this function manually feeds WDT to solve trigger problems
// if not used, CPU core dumps occur
void feedTheDog(){
  TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE; // write enable
  TIMERG0.wdt_feed=1;                       // feed dog
  TIMERG0.wdt_wprotect=0;                   // write protect
}
