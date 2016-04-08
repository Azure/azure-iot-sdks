package javaWrapper;

public class LoadTestLibrary 
{
    public static boolean test = false;
    
    public static boolean isTest()
    {
        return test;
    }
    
    LoadTestLibrary(boolean _test)
    {
        test = _test;
    }
    
}
