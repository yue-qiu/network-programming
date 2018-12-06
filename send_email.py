from smtplib import SMTP
from email.mime.text import MIMEText

email_from = "qiuyue9971@126.com"
email_recv = "867280434@qq.com"

content = "这是一封由python自动发送的邮件\n推荐你看一下这个网站www.qiuyueqy.com\n"
msg = MIMEText(content, "plain", "utf-8")
msg["From"] = email_from
msg["To"] = email_recv
msg["Subject"] = "python发送邮件"
passwd = "xxxxx"
HOST = "smtp.126.com"
PORT = 25

email = SMTP(HOST, PORT)
email.ehlo()
email.starttls()
email.ehlo()

email.login(email_from, passwd)
email.sendmail(email_from, email_recv, msg.as_string())

print("发送成功！")
email.quit()
