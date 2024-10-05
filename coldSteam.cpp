//+------------------------------------------------------------------+
//|                                                    Coldsteam.mq5 |
//|                                  Copyright 2024, MetaQuotes Ltd. |
//|                                             https://www.mql5.com |
//+------------------------------------------------------------------+
#property copyright "Copyright 2024, MetaQuotes Ltd."
#property link      "https://www.mql5.com"
#property version   "1.00"
//+------------------------------------------------------------------+
//| Expert initialization function                                   |
//+------------------------------------------------------------------+




//--- input parameters
input int    k_period = 5;    // %K Period (5)
input int    d_period = 3;    // %D Period (3)
input int    slowing  = 3;    // Slowing (3)
input double lot_size = 0.1;  // Lot size
input double overbought = 80; // Overbought level
input double oversold   = 20; // Oversold level



//--- Global Variables
// Bollinger Bands Parameters
input int BandsPeriod = 25;             // Period for Bollinger Bands (20 is common)
input double BandsDeviation = 2.0;      // Standard deviation (default = 2.0)
input ENUM_TIMEFRAMES Timeframe = PERIOD_M1;
int Stochastic_Handle;  // Handle for the stochastic indicator
int activebuyZone = 0;
int activeSellZOne = 0;
datetime lastBarTime = 0;
int activeSell = 0;
int activebuy = 0;
int startCount = 0;
double binaryBalance = 100;
int losses = 0;
int wins = 0; 
int stake = 1;
double MAValue;
double prevMAValue;
input int    MAPeriod  = 50;          // Period for Moving Average
input int    MAShift   = 0; 
double upperBand[], middleBand[], lowerBand[];
// URL of the API endpoint
input string apiUrlCall = "https://api-xig3blnaca-uc.a.run.app/trade/R_10/CALL";  // Replace with actual API URL
input string apiUrlput = "https://api-xig3blnaca-uc.a.run.app/trade/R_10/PUT";
  string headers; // Additional headers if required
   char postData[]; // Data to be sent with the request
      // Variable to store the response
   char result[];     // Buffer for the returned data
   string resultHeader;  // HTTP status code returned by the API

// Request timeout (in milliseconds)
input int timeout = 5000;  // 5 seconds



int OnInit()
  {
   // Define array sizes
   ArraySetAsSeries(upperBand, true);
   ArraySetAsSeries(middleBand, true);
   ArraySetAsSeries(lowerBand, true);
// Create Stochastic Oscillator handle
   Stochastic_Handle = iStochastic(_Symbol, _Period, k_period, slowing, d_period, MODE_SMA, 0);

   // Check if the handle is created successfully
   if (Stochastic_Handle == INVALID_HANDLE)
     {
      Print("Error creating Stochastic handle: ", GetLastError());
      return(INIT_FAILED);
     }
   return(INIT_SUCCEEDED);
  }
  
  
  
  
  
  void postCall(){
    int err;


string postData = "{\"startingAmount\":1}";  // JSON payload for the POST request
char postDataArray[];
StringToCharArray(postData, postDataArray);
// Send POST request
 
   
  err =   WebRequest(
      "POST",           // HTTP method
    apiUrlCall,              // URL
      "",          // headers 
      timeout,          // timeout
       postDataArray,          // the array of the HTTP message body
result,        // an array containing server response data
  resultHeader   // headers of server response
   );
   // Check for errors
   if (err != 0)
     {
      printf("Error sending request: %d", err);
      //return;
     }else{
     Print("Called deriv");
     
     }
   
 
  }


//+------------------------------------------------------------------+
//| Expert deinitialization function                                 |
//+------------------------------------------------------------------+
void OnDeinit(const int reason)
  {

    // Release indicator handle
   IndicatorRelease(Stochastic_Handle);
  }
  
//+------------------------------------------------------------------+
//| Expert tick function                                             |
//+------------------------------------------------------------------+
void OnTick()
  {
 
    
  watchZone("check");
   // Get the time of the last closed bar (index 1)
   datetime currentBarTime = TimeCurrent();
   // Convert the time to a string with the TIME_SECONDS format (hh:mm:ss)
   string timeString = TimeToString(currentBarTime, TIME_SECONDS);
   
    // Extract only the seconds part from the time string
   string secondsString = StringSubstr(timeString, 6, 2); 
    Print("Current Seconds: ", secondsString);
    // Print the extracted seconds
    if(secondsString == "58"){
    
    if(activeSell == 0){
    
    if(activeSellZOne == 1 && isSellorBuy(0)== 0 && (upperBand[1]<upperBand[14])){
    
    
    activeSellZOne = 0; //reset zone
    Print("sell>>>>>>>>>>>>>");
    
    activeSell = 1;
    
    }
    
    }
    
    else{
    
    activeSellZOne = 0; //reset zone  ongoing trade
   
    }
   
    if(activebuy == 0){
    //active buy once
    
     if(activebuyZone == 1 && isSellorBuy(0)== 1 && (lowerBand[1]>lowerBand[14])){
     
     activebuyZone = 0;// reset zone
     
    Print("buy>>>>>>>>>>>>>>>>");
    activebuy = 1;
    }
    
    }
    
    else{
    
    activebuyZone = 0;// reset zone ongoing trade
  
    }
    
  
    
    
   
    }
    
    if((StringToInteger(secondsString) % 5) == 0){
    
    Print("g");
      postCall();
    }
    
    
    
     if(secondsString == "00"){
     
   
     //new candle
     
     //take trade 
    if(startCount <= 7){
    
    //count till 6
     if(activebuy == 1 || activeSell == 1){//track either buy or sell trade
    startCount++;
    Print("tracking.....................");
    if(startCount == 7){
    //get trade result simulated
    if(activebuy == 1){
    //i.e it is a buy trade
    //index 0 is the sixth candle to analyse  index 5 is the first candle;
    for(int i=5;i>=0;i--)
      {
      if(isSellorBuy(i)== 1){
      //trade won
       wins++;
       
       if(wins <= 3){
         binaryBalance  = binaryBalance + (0.95* stake);
       }
         stake =  1;
    Print("current balance: ", binaryBalance);
     
      }else{
      //trade lost
      losses++;
      binaryBalance = binaryBalance - stake;
      if(losses == 1){
      stake =  2;
      }
       Print("current balance: ", binaryBalance);
      if(losses == 3){
      
      break;
     
      }
      
      }
       
      }
    
    
    
    }else{
    //i.e it is a sell trade
   
    //index 0 is the sixth candle to analyse  index 5 is the first candle;
    for(int i=5;i>=0;i--)
      {
      if(isSellorBuy(i)== 0){
      //trade won
       wins++;
       
       if(wins <= 3){
         binaryBalance  = binaryBalance + (0.95* stake);
       }
         stake =  1;
    Print("current balance: ", binaryBalance);
     
      }else{
      //trade lost
      losses++;
      binaryBalance = binaryBalance - stake;
      if(losses == 1){
      stake =  2;
      }
       Print("current balance: ", binaryBalance);
      if(losses == 3){
      
      break;
     
      }
      
      }
       
      }
    
    }
    
    }
    
    }
    
    }
    else{
    //end trade and reset active buy
    activebuy = 0;
    startCount = 0;
    
    }
   
     
     }
      
  //    Print("active buyZone:",activebuyZone);
   //    Print("active sellZone:",activeSellZOne);
   
  }
//+------------------------------------------------------------------+







int isSellorBuy(int index){

double openPricePrevious = iOpen(_Symbol, _Period, index);
   double closePricePrevious = iClose(_Symbol, _Period, index);
   // Check if the previous candle was a buy (bullish) or sell (bearish)
   if (closePricePrevious > openPricePrevious)
     {
      Print("Previous candle was a BUY (bullish) candle.");
      return 1;
     }
   else if (closePricePrevious < openPricePrevious)
     {
      Print("Previous candle was a SELL (bearish) candle.");
      return 0;
     }
   else
     {
      Print("Previous candle was a doji (open = close).");
      return 3;
     }

}


int watchZone(string type){

 // Copy Bollinger Bands data
   if (CopyBuffer(iBands(_Symbol, Timeframe, BandsPeriod, 0, BandsDeviation, PRICE_CLOSE ), 1, 0, 15, upperBand) <= 0)
     {
      Print("Error retrieving upper Bollinger Band data");
      return 0;
     }
   if (CopyBuffer(iBands(_Symbol, Timeframe, BandsPeriod, 0, BandsDeviation, PRICE_CLOSE ), 0, 0, 15, middleBand) <= 0)
     {
      Print("Error retrieving middle Bollinger Band data");
      return 0;
     }
   if (CopyBuffer(iBands(_Symbol, Timeframe, BandsPeriod, 0, BandsDeviation, PRICE_CLOSE), 2, 0, 15, lowerBand) <= 0)
     {
      Print("Error retrieving lower Bollinger Band data");
      return 0;
     }
     
    


// Buffers to store Stochastic values
   double k_value[], d_value[];
      // Get the Moving Average value for current and previous bars
 //  MAValue = iMA(_Symbol, _Period, MAPeriod, MAShift, MODE_SMA, PRICE_CLOSE, 0);
  // prevMAValue = iMA(_Symbol, _Period, MAPeriod, MAShift, MODE_SMA, PRICE_CLOSE, 1);

    // Get the Stochastic values for the current and previous candle
   if (CopyBuffer(Stochastic_Handle, 0, 0, 2, k_value) < 0 ||
       CopyBuffer(Stochastic_Handle, 1, 0, 2, d_value) < 0) 
     {
      Print("Error getting Stochastic values: ", GetLastError());
      return 0;
     }
     
     if(type == "check"){
      if(k_value[1] < oversold ){
     activebuyZone = 1;
     
     }else{
    // activebuyZone = 0;
     }
     if(k_value[1] > overbought){
     activeSellZOne = 1;
     }else{
   // activeSellZOne = 0;
     }
     
     return 0;
     
     }else{
     //confirm
      if(k_value[2] < oversold || k_value[2] > overbought){
    return 1;
     
     }else{
     return 0;}
     
     
     }
    
}