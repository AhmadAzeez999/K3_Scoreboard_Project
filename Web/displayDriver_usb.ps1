#url to post too(
$url = "http://localhost:8080/";

$runTicker = $true;
$timeDiff = 0;
$startTime = 0;
$stopTime = 0;
$nameMode = ' ';

#main configs always on
# Http Server
$http = [System.Net.HttpListener]::new(); 

# Hostname and port to listen on
$http.Prefixes.Add($url);

# Start the Http Server 
$http.Start();


#serial specific port data------------------------------------------ #6

$ports = (Get-WmiObject -query "SELECT * FROM Win32_PnPEntity" | Where {$_.Name -Match "COM\d+"}).name;
#default sign port...program should read correct port if sign plugged in
$signPort = "COM14"

foreach($p in $ports){
    if($p -Match "Arduino Uno"){
     $p = $p.replace("Arduino Uno", "");
     $p = $p.replace("(","");
     $p = $p.replace(")","");
     $signPort = $p.trim();
    }
}

$signPort = "COM" 
$enteredPort = Read-Host "Enter Comm PORT";
$signPort = $signPort + $enteredPort;

$baudRate = Read-Host "Enter Baud Rate";

# Log ready message to terminal 
if ($http.IsListening) {
   # write-host "Display Driver is Running!  " -f 'black' -b 'gre'
   write-host "Connected to the Scoreboard!  " -f 'black' -b 'gre'
   write-host " Please Minimize this Window!  " -f 'black' -b 'gre'
};


#           1200               9600  
#Write-Host "Com Port:" $signPort
$out_port_a= new-Object System.IO.Ports.SerialPort $signPort,$baudRate,None,8,one;
$out_port_a.open();

Start-Sleep -s 1


$displaySelect = 0;  # 0=All, 1=a, 2=b, 3=c, 4=a/b, 5=a/c, 6=b/c
$resultsDisplaySelect = 1;
$tickerDisplaySelect = 2;

#---------------- MAIN PROGRAM----------------------

function send-data-to-display($data){
        send-data-local($data);
}

function send-from-timing-system($data){
        send-from-timing-system-local($data);
};

function send-from-ticker($data){
        send-from-ticker-local($data);
};

#send data to display, write it console, write it to log
function send-data-local($data){
    $out_port_a.WriteLine($data);      
    
    write-host($data); 
   # Add-Content C:\display\logs\serial_data.txt $data;
};

function send-from-timing-system-local($data){
    $out_port_a.WriteLine($data);
    write-host("kevinkevin");
   write-host($data); 
   # Add-Content C:\K3_Scoreboard\logs\serial_data.txt $data;
};

function send-from-ticker-local($data){
    $out_port_a.WriteLine($data);
  #  write-host($data); 
    #Add-Content C:\display\logs\serial_data.txt $data;
};


#accepts a object with command and data value
function pad-text($value){

    
    #defualt command is to pause   
   
   # write-host($value);    
    $cmd = "*";

    #send control char on what to do with captured text
    if($value.command -eq 'scroll1W'){
        $cmd = "A";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'pause1W'){
        $cmd = "B";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'static1W'){
        $cmd = "C";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }

    elseif($value.command -eq 'scroll1'){
        $cmd = "a";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'pause1'){
        $cmd = "b";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'static1'){
        $cmd = "c";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }

    elseif($value.command -eq 'scroll2W'){
        $cmd = "D";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'pause2W'){
        $cmd = "E";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'static2W'){
        $cmd = "F";
        $returnValue = "[z" + $value.data + "]" + $cmd;
    }

    elseif($value.command -eq 'scroll2'){
        $cmd = "d";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'pause2'){
        $cmd = "e";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'static2'){
        $cmd = "f";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }

     elseif($value.command -eq 'scroll3W'){
        $cmd = "G";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'pause3W'){
        $cmd = "H";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'static3W'){
        $cmd = "I";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }

     elseif($value.command -eq 'scroll3'){
        $cmd = "g";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'pause3'){
        $cmd = "h";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'static3'){
        $cmd = "i";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }

     elseif($value.command -eq 'scroll4W'){
        $cmd = "J";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'pause4W'){
        $cmd = "K";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'static4W'){
        $cmd = "L";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }

     elseif($value.command -eq 'scroll4'){
        $cmd = "j";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'pause4'){
        $cmd = "k";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'static4'){
        $cmd = "l";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }

     elseif($value.command -eq 'scrollTW'){
        $cmd = "M";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'pauseTW'){
        $cmd = "N";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'staticTW'){
        $cmd = "O";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }

      elseif($value.command -eq 'scrollT'){
        $cmd = "m";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'pauseT'){
        $cmd = "n";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'staticT'){
        $cmd = "o";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }

     elseif($value.command -eq 'scrollBW'){
        $cmd = "P";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'pauseBW'){
        $cmd = "Q";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'staticBW'){
        $cmd = "R";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }

      elseif($value.command -eq 'scrollB'){
        $cmd = "p";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'pauseB'){
        $cmd = "q";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'staticB'){
        $cmd = "r";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }

     elseif($value.command -eq 'scrollBigW'){
        $cmd = "S";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'pauseBigW'){
        $cmd = "T";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'staticBigW'){
        $cmd = "U";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }

     elseif($value.command -eq 'scrollBig'){
        $cmd = "s";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'pauseBig'){
        $cmd = "t";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }elseif($value.command -eq 'staticBig'){
        $cmd = "u";
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }

    else{
        $returnValue = "[ " + $value.data + "]" + $cmd;
    }
    #add square brackets to encapsulate text and add before and after spaces to clean display
    return $returnValue;
};


#infinte loop of listening to URLs
while ($http.IsListening) {
    #get the URL that was sent
    $context = $http.GetContext();

    #GET and default URL
    if ($context.Request.HttpMethod -eq 'GET' -and $context.Request.RawUrl -eq '/') {

        #resposed to the request
        $buffer = [System.Text.Encoding]::UTF8.GetBytes($html); # convert htmtl to bytes
        $context.Response.ContentLength64 = $buffer.Length;
        $context.Response.AppendHeader("Access-Control-Allow-Origin","*");
        $context.Response.OutputStream.Write($buffer, 0, $buffer.Length); #stream to broswer
        $context.Response.OutputStream.Close(); # close the response
    };

    #POST to serial URL
    if ($context.Request.HttpMethod -eq 'POST' -and $context.Request.RawUrl -eq '/serial/post') {
        #get the data that was posted
        $FormContent = [System.IO.StreamReader]::new($context.Request.InputStream).ReadToEnd();

        if($FormContent -match "nameMode"){
            $FormContent = $FormContent.Replace("nameMode", "");
            $arrayOfForms = $FormContent.split('=');
            if($nameMode -eq 1){
                $FormContent = $arrayOfForms[1];
            }
            else{
                $FormContent = $arrayOfForms[0];
            }
        } 

       send-from-timing-system($FormContent);
       Write-Output([int][char]$FormContent[4]);

       

        #resposed to the request
        $buffer = [System.Text.Encoding]::UTF8.GetBytes("");
        $context.Response.ContentLength64 = $buffer.Length;
        $context.Response.AppendHeader("Access-Control-Allow-Origin","*");
        $context.Response.OutputStream.Write($buffer, 0, $buffer.Length);
        $context.Response.OutputStream.Close(); 
    };

     #POST to Ticker URL
    if ($context.Request.HttpMethod -eq 'POST' -and $context.Request.RawUrl -eq '/ticker/post') {
        #get the data that was posted
        $FormContent = [System.IO.StreamReader]::new($context.Request.InputStream).ReadToEnd();

        if($runTicker){
            send-from-ticker($FormContent);
        }


        #resposed to the request
        $buffer = [System.Text.Encoding]::UTF8.GetBytes("");
        $context.Response.ContentLength64 = $buffer.Length;
        $context.Response.AppendHeader("Access-Control-Allow-Origin","*");
        $context.Response.OutputStream.Write($buffer, 0, $buffer.Length);
        $context.Response.OutputStream.Close(); 
    };

    #POST to dashboard 
    if ($context.Request.HttpMethod -eq 'POST' -and $context.Request.RawUrl -eq '/server/command') {

        #get the data that was posted
        $FormContent = [System.IO.StreamReader]::new($context.Request.InputStream).ReadToEnd();

        #web data is sent via Json, convert from Json
        $data = $FormContent | ConvertFrom-Json 

        #future logic, all data just sent to display
        if($data.command -eq 'tickerDisplay'){
           $tickerDisplaySelect = $data.data;
        }
        
        if($data.command -eq 'resultsDisplay'){
            $resultsDisplaySelect = $data.data;
        }

        if($data.command -eq 'nameMode'){
            $nameMode = $data.data;
            if($nameMode -eq 1){
                send-data-to-display("[1First NameLast Name ]<");
            }
            else{
                send-data-to-display("[ Fint.LnamePlc.  Rnd. ]<");
            }
        }


        #resposed to the request
        $buffer = [System.Text.Encoding]::UTF8.GetBytes("");
        $context.Response.ContentLength64 = $buffer.Length;
        $context.Response.AppendHeader("Access-Control-Allow-Origin","*");
        $context.Response.OutputStream.Write($buffer, 0, $buffer.Length);
        $context.Response.OutputStream.Close();
    };

    #POST to dashboard URL
    if ($context.Request.HttpMethod -eq 'POST' -and $context.Request.RawUrl -eq '/dashboard/post') {

        #get the data that was posted
        $FormContent = [System.IO.StreamReader]::new($context.Request.InputStream).ReadToEnd();

        #web data is sent via Json, convert from Json
        $dataToSend = $FormContent | ConvertFrom-Json 

        $displaySelect = $dataToSend.displaySelect;

        #future logic, all data just sent to display
        if($dataToSend.command -eq 'changeTicker'){
            $runTicker = !$runTicker;
            write-host $runTicker
            write-host "ticker status change"
        }
        elseif($dataToSend.command -eq 'postGraphic'){
            send-data-to-display($dataToSend.data);
        }
        elseif($dataToSend.command -eq 'colorChange'){
            send-data-to-display($dataToSend.data);
        }
        elseif($dataToSend.command -eq 'postRaw'){
            send-data-to-display($dataToSend.data);
        }
        elseif($dataToSend.command -eq 'fontChange'){
            send-data-to-display($dataToSend.data);
        }
        elseif($dataToSend.command -eq 'loadDefault'){
            send-data-to-display($dataToSend.data);
        }
        else{
            $nonJsonData = pad-text($dataToSend);
            send-data-to-display($nonJsonData);
        };

        #resposed to the request
        $buffer = [System.Text.Encoding]::UTF8.GetBytes("");
        $context.Response.ContentLength64 = $buffer.Length;
        $context.Response.AppendHeader("Access-Control-Allow-Origin","*");
        $context.Response.OutputStream.Write($buffer, 0, $buffer.Length);
        $context.Response.OutputStream.Close();
    };

    #POST to timing system URL
    if ($context.Request.HttpMethod -eq 'POST' -and $context.Request.RawUrl -eq '/timer/post') {

        #get the data that was posted
        $received = [System.IO.StreamReader]::new($context.Request.InputStream).ReadToEnd();
        $FormContent = $received.Substring(1, $received.Length -1);
       # Write-Output $FormContent;

        
        $displaySelect = $received.Substring(0,1);

        if($FormContent[0] -eq '#'){
         $startTime = Get-Date;
        }
        
        if($formContent[0] -eq '&'){
            $stopTime = Get-Date;
            $timeDiff = NEW-TimeSpan -Start $startTime -End $stopTime;
        }

        if($formContent[0] -eq '$'){
           if($timeDiff.Minutes -le 9){
                $numOfMins = '0' + $timeDiff.Minutes;
           }
           else{
            $numOfMins = $timeDiff.Minutes;
           }

           if($timeDiff.Seconds -le 9){
                $numOfSeconds = '0' + $timeDiff.Seconds;
           }
           else{
            $numOfSeconds = $timeDiff.Seconds;
           }

            
           if($timeDiff){
                $numOfMillis = $timeDiff.milliseconds.ToString().Substring(0,2);
           }
           

         $formContent = $FormContent[0] + $numOfMins + $numOfSeconds + $numOfMillis;
        }

        #just send data straight to display, format from javascript
        send-data-to-display($FormContent);

        #resposed to the request
        $buffer = [System.Text.Encoding]::UTF8.GetBytes("");
        $context.Response.ContentLength64 = $buffer.Length;
        $context.Response.AppendHeader("Access-Control-Allow-Origin","*");
        $context.Response.OutputStream.Write($buffer, 0, $buffer.Length);
        $context.Response.OutputStream.Close();
    };

    #POST to dashboard URL
    if ($context.Request.HttpMethod -eq 'POST' -and $context.Request.RawUrl -eq '/display/assist') {

        #get the data that was posted
        $FormContent = [System.IO.StreamReader]::new($context.Request.InputStream).ReadToEnd();

        $displayAssist = $FormContent.Substring(1,$FormContent.Length -1); 
        
        $displaySelect = $FormContent.Substring(0,1);    

        send-data-to-display($displayAssist);

        #resposed to the request
        $buffer = [System.Text.Encoding]::UTF8.GetBytes("");
        $context.Response.ContentLength64 = $buffer.Length;
        $context.Response.AppendHeader("Access-Control-Allow-Origin","*");
        $context.Response.OutputStream.Write($buffer, 0, $buffer.Length);
        $context.Response.OutputStream.Close();
    };

    #POST kill
    if ($context.Request.HttpMethod -eq 'POST' -and $context.Request.RawUrl -eq '/kill') {

        #resposed to the request
        $buffer = [System.Text.Encoding]::UTF8.GetBytes("");
        $context.Response.ContentLength64 = $buffer.Length;
        $context.Response.AppendHeader("Access-Control-Allow-Origin","*");
        $context.Response.OutputStream.Write($buffer, 0, $buffer.Length);
        $context.Response.OutputStream.Close();

        #stop web listener
        $http.Stop();
    };
};