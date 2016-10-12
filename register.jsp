<%@ page language="java" import="java.util.*" contentType="text/html;charset=gb2312"%>
<html>
<head>
<title>表单输入验证示例</title>
<script type="text/javascript">
function validate()
{
var username=document.forms[0].username.value;
var password=document.forms[0].password.value;
var rePassword=document.forms[0].rePassword.value;
if(username.length<=0)
alert("用户名不能为空！ ");
else if(password<=0)
alert("密码不能为空！ ");
else if(rePassword.length<=0)
alert("重新输入密码不能为空！ ");
else if(username.length<4)
alert("用户名不能小于 4 位！ ");
else if(password!=rePassword)
alert("两次输入密码不一致！ ");
else
{
document.forms[0].submit();
}
}
</script>
</head>
<body>
<form action="register_act.jsp" method="post">
用户名：<input type="text" name="username"></input><br>
密码：  <input type="password" name="password"></input><br>
重新输入密码:<input type="password" name="rePassword"></input><br>
<input type="button" value="提交" onClick="validate()"/>
<input type="reset" value="取消" />
</form>
</body>
</html>
ii
