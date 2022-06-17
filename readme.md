# 需求
编写一个简易的交互式员工信息录入程序，实现员工信息的插入、删除、修改、查询、遍历。具体要求为：
1. 程序启动后是交互式运行的（类似shell，等待用户输入指令，然后完成对应的功能）
2. 程序支持的指令名和参数格式自定，但必须支持下面几种类型：
	(1) 插入：支持添加新员工，员工信息包括：姓名（可重复）、工号（唯一）、入职时间、部门、职位；录入的员工信息不要求持久化（保存在内存即可）
	(2) 删除：支持根据工号，删除对应的员工及相关信息
	(3) 修改：支持修改已录入的员工信息（工号除外）
	(4) 查询：支持根据姓名或工号查询对应员工信息
	(5) 遍历：支持遍历打印所有已录入的员工信息，遍历时，支持按任意内容过滤（如只输出某部门的员工、只输出某职位的员工）和排序（如按照工号排序、按照入职时间排序）
	(6) 帮助：支持打印输出各命令的用法、参数等提示信息
3. 在不超过系统资源的前提下，不限制录入员工的数量
4. 不允许使用现成的数据库软件

# 实现
1. 支持常规的增删改查操作，支持查询时的过滤及排序
2. 支持本地查询或远程连接查询，程序绑定端口为16166
3. 本程序目前不支持并发，全部操作均在主线程完成
```
Use ADD cmd to add a staff to the database.
	e.g. ADD 10086 name:Zhangsan date:2022-05-11 dept:ZTA pos:engineer
Use DEL cmd to remove a/all staff from the database.
	e.g. DEL 10086 to remove a staff, or DEL * to clear the database.
Use MOD cmd to modify a staff's info.
	e.g. MOD 10086 dept:CWPP name:Lisi
Use GET cmd to obtain a/all staff's info.
	e.g. GET 10086 to obtain a staff's info, or GET name:Lisi dept:ZTA to obtain one or more staff's info, or GET * to print all staff's info.
	If you want output being sorted, use --sort:xx, e.g. GET --sort:id * to sort output by staff id.
Use LOG cmd [local user only] to set log level.
	e.g. LOG debug to set log level to debug. Log level include [debug, info, error, fault, off].
The above commands are not case sensitive.
```
