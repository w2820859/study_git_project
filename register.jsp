<%@ page language="java" import="java.util.*" contentType="text/html;charset=gb2312"%>
<html>
<head>
<title>��������֤ʾ��</title>
<script type="text/javascript">
function validate()
{
var username=document.forms[0].username.value;
var password=document.forms[0].password.value;
var rePassword=document.forms[0].rePassword.value;
if(username.length<=0)
alert("�û�������Ϊ�գ� ");
else if(password<=0)
alert("���벻��Ϊ�գ� ");
else if(rePassword.length<=0)
alert("�����������벻��Ϊ�գ� ");
else if(username.length<4)
alert("�û�������С�� 4 λ�� ");
else if(password!=rePassword)
alert("�����������벻һ�£� ");
else
{
document.forms[0].submit();
}
}
</script>
</head>
<body>
<form action="register_act.jsp" method="post">
�û�����<input type="text" name="username"></input><br>
���룺  <input type="password" name="password"></input><br>
������������:<input type="password" name="rePassword"></input><br>
<input type="button" value="�ύ" onClick="validate()"/>
<input type="reset" value="ȡ��" />
</form>
</body>
</html>
ii
