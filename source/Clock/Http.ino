////////////////////////////////////////////////////////////////////////////////
// Http
////////////////////////////////////////////////////////////////////////////////

String httpHeader(const String &title)
{
  return
    String("<html>\n"
           "<head>\n"
           "<title>") +
    title +
    String("</title>\n"
           "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\n"
           "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"/>\n"
           "</head>\n"
           "<body>\n"
           "  <h1>") +
    title +
    String("</h1>\n") ;
}
String httpFooter()
{
  return
    String("</body>\n"
           "</html>\n") ;
}

void httpOnRoot()
{
  httpServer.send(200, "text/html",
                  httpHeader("Clock") +
                  t.toString() +
                  httpFooter()) ;
}

bool valid(const String &s, unsigned long &v, unsigned long max)
{
  unsigned long t ;

  switch (s.length())
  {
  case 1:
    if (('0' <= s[0]) && (s[0] <= '9'))
    {
      t = s[0] - '0' ;
      if (t <= max)
      {
        v = t ;
        return true ;
      }
    }
    break ;
  case 2:
    if (('0' <= s[0]) && (s[0] <= '9') &&
        ('0' <= s[1]) && (s[1] <= '9'))
    {
      t = (s[0] - '0') * 10 + (s[1] - '0') ;
      if (t <= max)
      {
        v = t ;
        return true ;
      }
    }
    break ;
  }
  return false ;
}

String httpOnClockRow(const String &name, const String &tag, const String &max, uint8_t value, const Color &color)
{
  char buff[16] ;
  sprintf(buff, "#%02x%02x%02x", color._r, color._g, color._b) ;
  
  return
    String("<tr><td>") +
    name +
    String("</td><td>"
           "<input name=\"") +
    tag +
    String("\" type=\"number\" value=\"") +
    String(value) +
    String("\" min=\"0\" max=\"") +
    max +
    String("\"/>"
           "</td><td>"
           "<input name=\"col") +
    tag +
    String("\" type=\"color\" value=\"") +
    String(buff) +
    String("\"/>"
           "</td></tr>\n") ;
}

void httpOnClock()
{
  unsigned long h, m, s ;
  if (httpServer.hasArg("h") && valid(httpServer.arg("h"), h, 23) &&
      httpServer.hasArg("m") && valid(httpServer.arg("m"), m, 59) &&
      httpServer.hasArg("s") && valid(httpServer.arg("s"), s, 59))
  {
    t.set(h, m, s) ;
  }

  if (httpServer.hasArg("colH"))
  {
    // todo
    settings.save() ;
  }

  httpServer.send(200, "text/html",
                  httpHeader("Clock - Settings") +
                  String("<form action=\"clock.html\">\n"
                         "<table>\n") +
                  httpOnClockRow("Hour"  , "h", "23", t._hour  , settings._colHour) +
                  httpOnClockRow("Minute", "m", "59", t._minute, settings._colMinute) +
                  httpOnClockRow("Second", "s", "59", t._second, settings._colSecond) +
                  String("</table>\n"                  
                         "<button type=\"submit\">Zeit setzen</button>\n"
                         "</form>\n") +
                  httpFooter()) ;
}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
