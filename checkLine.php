<?php

$beginPos   = intval($argv[1]);
$endPos     = intval($argv[2]);
echo "开始导入第".$beginPos."至".$endPos."条\n";
$rootDir = dirname(__FILE__);
$recordPath = $rootDir."/data/checkLine/".$beginPos."_tmpRecord.json";
$file   = file_get_contents("./data/record_0917.log");
$lines  = explode("\n",$file);
$cnt    = 0;
$pos    = 0;
$btime  = time();
$questionIDs = array();
foreach($lines as $line){
    if($pos<$beginPos){
        $pos++;
        continue;
    }
    if($pos>=$endPos){
        break;
    }
    $pos++;
    list($id,$uid,$distance,$ossUrl,$sportType,$channel) = explode("\t",$line);
    try {
        echo "处理id为".$id;
        if($sportType==1){
            echo ",室内模式,不处理\n";
            continue;
        }
        if(!empty($ossUrl)&&$ossUrl!="NULL"){
            $out    = array();
            $record = file_get_contents($ossUrl);
            file_put_contents($recordPath,$record);
            exec("cd ./build/src;./CheckLine $recordPath", $out);
            if($out[0]!="Checkline ret: 0"){
                $questionIDs[$id] = ltrim($out[0],"Checkline ret: ");
                echo $out[0]."数据有问题";
            }
        }
        echo "\n";
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
foreach($questionIDs as $k=>$v){
    echo $k.",".$v."\n";
}
