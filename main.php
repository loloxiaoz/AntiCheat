<?php

$beginPos   = intval($argv[1]);
$endPos     = intval($argv[2]);
echo "开始导入第".$beginPos."至".$endPos."条\n";
$rootDir = dirname(__FILE__);
$recordPath = $rootDir."/data/".$beginPos."_tmpRecord.json";
#$outputBasePath = $rootDir."/data/".$beginPos."_";
$outputBasePath = $rootDir."/output/";
$file   = file_get_contents("./data/record.log");
$lines  = explode("\n",$file);
$cnt    = 0;
$pos    = 0;
$btime  = time();
foreach($lines as $line){
    if($pos<$beginPos){
        $pos++;
        continue;
    }
    if($pos>=$endPos){
        break;
    }
    $pos++;
    list($id,$uid,$starttime,$distance,$ossUrl) = explode("\t",$line);
    try {
        if(!empty($ossUrl)){
            $record = file_get_contents($ossUrl);
            file_put_contents($recordPath,$record);
            if($distance<10000){
                $outputPath = $outputBasePath."0.geojson";
            }else if($distance<21000){
                $outputPath = $outputBasePath."10000.geojson";
            }else if($distance<42000){
                $outputPath = $outputBasePath."21000.geojson";
            }else{
                $outputPath = $outputBasePath."42000.geojson";
            }
            echo "处理id为".$id."\n";
            exec("cd ./build/src;./AntiCheat $recordPath $outputPath");
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
        echo '执行'.$cnt.'条,耗时'.$costTime."秒\n";
        $btime = time();
    }
}
