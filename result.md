# 单元测试
单元测试覆盖所有源代码文件，总体行覆盖率为88.9%，函数覆盖率为100%
详细信息见～employee_manager/work/code_cov_report

# 内存泄漏检测
因macOS12无法安装valgind(valgind仅支持macOS10.12及一下系统)，使用Xcode的Instruments工具进行内存泄漏测试，测试通过
测试记录见～employee_manager/work/leaks_test_report.trace

# GDB调试输出数据库所有记录
macOS无GDB调试工具，使用LLDB进行调试，完成所有记录的输出
调试记录见～employee_manager/work/lldb_result