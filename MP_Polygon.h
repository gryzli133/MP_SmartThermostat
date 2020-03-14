double mapDouble(double x, double in_min, double in_max, double out_min, double out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

double MP_Polygon(double value, double X1, double X2, double X3, double X4, double Y1, double Y2, double Y3, double Y4)
{
  if(X1<X2 && X2<X3 && X3<X4)
  {
    if(value < X2)
    {
      value = mapDouble(value, X1, X2, Y1, Y2);
    }
    else
    {
      if(value < X3)
      {
        value = mapDouble(value, X2, X3, Y2, Y3);
      }
      else
      {
        value = mapDouble(value, X3, X4, Y3, Y4);
      }
    }
    return value;
  }
  else
  {
    return value;
  } 
}


