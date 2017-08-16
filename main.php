<?php

$rootDir = dirname(__FILE__);
$recordPath = $rootDir."/data/tmpRecord.json";
#$file   = file_get_contents("./data/record.log");
$file   = file_get_contents("./data/lp_record.log");
$lines  = explode("\n",$file);
$cnt    = 0;
$btime  = time();
foreach($lines as $line){
    list($id,$uid,$starttime,$ossUrl) = explode("\t",$line);
    try {
        if(!empty($ossUrl)){
            $record = file_get_contents($ossUrl);
            file_put_contents($recordPath,$record);
            exec("cd ./build/src;./AntiCheat $recordPath");
        }
    }catch(Exception $e){
        file_put_contents("run.log",$id."下载失败",FILE_APPEND);
    }
    finally{
        @unlink($recordPath);
    }
    $cnt++;
    if($cnt%10==0){
        $etime = time();
        $costTime = $etime-$btime;
        echo '执行100条,耗时'.$costTime."秒\n";
        $btime = time();
    }
    if($cnt==1000){
        break;
    }
}
