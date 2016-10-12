<%@ page language="java" import="java.util.*" contentType="text/html;charset=gb2312"%>
<%@ page import="java.sql.*"%>
<html>
<head>
<title>DBLoginCheck</title>
</head>
<%!
 public static final String DBDRIVER = "com.mysql.jdbc.Driver" ;
 public static final String userName = "root"; 
 public static final String userPasswd = "9527";
 public static final String dbName = "user"; 
 public static final String tableName="account"; 
 public static final String DBURL = "jdbc:mysql://127.0.0.1:3306/"+dbName;
%>

<%
  Connection conn = null ;
  PreparedStatement pstmt = null ;
  ResultSet rs = null ;
  boolean ifexist = false ; 

%>
<body>
<font size="2">
<%
 String username = request.getParameter("username") ; 
 String userpassword = request.getParameter("password") ; 

 Class.forName(DBDRIVER) ;
 conn = DriverManager.getConnection(DBURL,userName,userPasswd) ;
 String sql = "select * from "+tableName +" where user_name = '"+username+"'" ;
 pstmt = conn.prepareStatement(sql) ;
 rs = pstmt.executeQuery() ;

 while(rs.next()){
        ifexist=true;
        out.print("The user is already exist!");
 %>            
        <a href="register.jsp">Click here return</a>
<% }
//   conn.close() ;
%>

<%
  if(!ifexist){
   sql="insert into account(user_ID, user_name, user_password) values(NULL, '"+username+"','"+userpassword+"')";
   pstmt = conn.prepareStatement(sql) ;
   int i = pstmt.executeUpdate() ;
   out.print("Register successfully!");
%>            
           <a href="DBLogin.jsp">Click here return</a>
<% 
}
%>


</font>
</body>
</html>
