# 中国节假日、工作日、休息日 接口

## 获取指定日期的节假日信息

1、接口地址：http://api.goseek.cn/Tools/holiday?date=数字日期，支持https协议。   
2、返回数据：工作日对应结果为 0, 休息日对应结果为 1, 节假日对应的结果为 2   
3、节假日数据说明：本接口包含2017年起的中国法定节假日数据，数据来源国务院发布的公告，每年更新1次，确保数据最新   
4、示例：   
http://api.goseek.cn/Tools/holiday?date=20170528   
https://api.goseek.cn/Tools/holiday?date=20170528   
返回数据：   
`{"code":10001,"data":2}`  
