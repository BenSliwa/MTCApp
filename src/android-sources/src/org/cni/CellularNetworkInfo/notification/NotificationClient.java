package org.cni.CellularNetworkInfo.notification;

import android.content.Context;

import android.telephony.CellInfo;
import android.telephony.CellInfoCdma;
import android.telephony.CellInfoGsm;
import android.telephony.CellInfoLte;
import android.telephony.CellIdentityLte;
import android.telephony.CellIdentityGsm;
import android.telephony.CellIdentityCdma;
import android.telephony.CellSignalStrengthCdma;
import android.telephony.CellSignalStrengthGsm;
import android.telephony.CellSignalStrengthLte;
import android.telephony.TelephonyManager;


import android.net.ConnectivityManager;
import android.net.NetworkInfo;

import org.json.JSONArray;
import org.json.JSONObject;
import org.json.JSONException;

import android.os.Environment;


import java.lang.reflect.Method;
import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.telephony.PhoneStateListener;
import android.telephony.SignalStrength;
import android.telephony.TelephonyManager;
import android.util.Log;

public class NotificationClient extends org.qtproject.qt5.android.bindings.QtActivity
{
    private static NotificationClient m_instance;
    private static boolean m_init = false;

    private SignalStrength      signalStrength;
    private TelephonyManager    telephonyManager;
    private final static String LTE_TAG             = "LTE_Tag";
    private final static String LTE_SIGNAL_STRENGTH = "getLteSignalStrength";

    public NotificationClient()
    {
        m_instance = this;
    }



    @Override
    protected void onStart()
    {
        super.onStart();
        onInit();
    }

    public void onInit()
    {

        telephonyManager = (TelephonyManager) getSystemService(Context.TELEPHONY_SERVICE);

        // Listener for the signal strength.
        final PhoneStateListener mListener = new PhoneStateListener()
        {
            private String getSignalStrengthByName(SignalStrength signalStrength, String methodName)
            {
                try
                {
                    Class classFromName = Class.forName(SignalStrength.class.getName());
                    java.lang.reflect.Method method = classFromName.getDeclaredMethod(methodName);
                    Object object = method.invoke(signalStrength);

                    return object.toString();
                }
                catch (Exception ex)
                {
                    return "";
                }
            }



            @Override
            public void onSignalStrengthsChanged(SignalStrength sStrength)
            {
                super.onSignalStrengthsChanged(sStrength);
                signalStrength = sStrength;


                double snr_dB = Double.parseDouble(getSignalStrengthByName(signalStrength, "getLteRssnr")) / 10.0;
                int rsrp_dBm = Integer.parseInt(getSignalStrengthByName(signalStrength, "getLteRsrp"));
                int rsrq_dB = Integer.parseInt(getSignalStrengthByName(signalStrength, "getLteRsrq"));
                int dBm = Integer.parseInt(getSignalStrengthByName(signalStrength, "getLteDbm"));
                int cqi = Integer.parseInt(getSignalStrengthByName(signalStrength, "getLteCqi"));
                int level = Integer.parseInt(getSignalStrengthByName(signalStrength, "getLteLevel"));
                //int ta = Integer.parseInt(getSignalStrengthByName(signalStrength, "getLteTimingAdvance"));

                System.out.println("getLTEsignalStrength " + sStrength.toString());
                System.out.println("__TESTING__ dBm " + dBm);
                System.out.println("__TESTING__ RSRP " + rsrp_dBm);
                System.out.println("__TESTING__ RSRQ " + rsrq_dB);
                System.out.println("__TESTING__ SNR " + snr_dB);
                System.out.println("__TESTING__ CQI " + cqi);
                System.out.println("__TESTING__ Level " + level);
               // System.out.println("__TESTING__ TA " + ta);
            }
        };

        // Register the listener for the telephony manager
        telephonyManager.listen(mListener, PhoneStateListener.LISTEN_SIGNAL_STRENGTHS);
    }


    public static JSONObject parseGsm(CellInfo _info)
    {
        JSONObject cellInfoObject = new JSONObject();

        final CellInfoGsm cellInfo = (CellInfoGsm)_info;
        final CellIdentityGsm cellIdentity = cellInfo.getCellIdentity();
        final CellSignalStrengthGsm cellSignalStrength = cellInfo.getCellSignalStrength();

        try{
            cellInfoObject.put("isRegistered", cellInfo.isRegistered());

            //cellInfoObject.put("arfcn", cellIdentity.getArfcn()); // API 24
            //cellInfoObject.put("bsic", cellIdentity.getBsic());
            cellInfoObject.put("cid", cellIdentity.getCid());
            cellInfoObject.put("lac", cellIdentity.getLac());
            cellInfoObject.put("mcc", cellIdentity.getMcc());
            cellInfoObject.put("mnc", cellIdentity.getMnc());
            cellInfoObject.put("psc", cellIdentity.getPsc());

            cellInfoObject.put("asuLevel", cellSignalStrength.getAsuLevel());
            cellInfoObject.put("dBm", cellSignalStrength.getDbm());
            cellInfoObject.put("level", cellSignalStrength.getLevel());

            cellInfoObject.put("type", "GSM");


        }
        catch(JSONException ex){
            System.out.println("JSON error");
        }

        return cellInfoObject;
    }

    public static JSONObject parseCdma(CellInfo _info)
    {
        JSONObject cellInfoObject = new JSONObject();

        final CellInfoCdma cellInfo = (CellInfoCdma)_info;
        final CellIdentityCdma cellIdentity = cellInfo.getCellIdentity();
        final CellSignalStrengthCdma cellSignalStrength = cellInfo.getCellSignalStrength();

        try{
            cellInfoObject.put("basestationId", cellIdentity.getBasestationId());
            cellInfoObject.put("latitude", cellIdentity.getLatitude());
            cellInfoObject.put("longitude", cellIdentity.getLongitude());
            cellInfoObject.put("networkId", cellIdentity.getNetworkId());
            cellInfoObject.put("systemId", cellIdentity.getSystemId());

            cellInfoObject.put("asuLevel", cellSignalStrength.getAsuLevel());
            cellInfoObject.put("cdmaDbm", cellSignalStrength.getCdmaDbm());
            cellInfoObject.put("cdmaEcio", cellSignalStrength.getCdmaEcio());
            cellInfoObject.put("cdmaLevel", cellSignalStrength.getCdmaLevel());
            cellInfoObject.put("dBm", cellSignalStrength.getDbm());

            cellInfoObject.put("evdoDbm", cellSignalStrength.getEvdoDbm());
            cellInfoObject.put("evdoEcio", cellSignalStrength.getEvdoEcio());
            cellInfoObject.put("evdoLevel", cellSignalStrength.getEvdoLevel());
            cellInfoObject.put("evdoSnr", cellSignalStrength.getEvdoSnr());

            cellInfoObject.put("level", cellSignalStrength.getLevel());
            cellInfoObject.put("type", "CDMA");
        }
        catch(JSONException ex){
            System.out.println("JSON error");
        }

        return cellInfoObject;
    }

    private String getCellInfoByName(CellIdentityLte cell, String methodName)
    {
        try
        {
            Class classFromName = Class.forName(CellIdentityLte.class.getName());
            java.lang.reflect.Method method = classFromName.getDeclaredMethod(methodName);
            Object object = method.invoke(cell);

            return object.toString();
        }
        catch (Exception ex)
        {
            return "";
        }
    }

    public static JSONObject parseLte(CellInfo _info)
    {
        JSONObject cellInfoObject = new JSONObject();

        final CellInfoLte cellInfo = (CellInfoLte)_info;
        final CellIdentityLte cellIdentity = cellInfo.getCellIdentity();
        final CellSignalStrengthLte cellSignalStrength = cellInfo.getCellSignalStrength();


        try{
            //System.out.println("__EARFCN___" + getCellInfoByName(cellIdentity, "getEarfcn"));
            //System.out.println("__EARFCN___" + cellIdentity.toString());


            boolean isRegistered = cellInfo.isRegistered();
            cellInfoObject.put("isRegistered", isRegistered);

            int asuLevel = cellSignalStrength.getAsuLevel();
            int dBm = cellSignalStrength.getDbm();
            int level = cellSignalStrength.getLevel();
            int timingAdvance = cellSignalStrength.getTimingAdvance();


            String[] css = cellSignalStrength.toString().split(" ");
            String ss = css[1];
            ss = ss.replace("ss=", "");

           // String rsrp = css[2];
           // rsrp = rsrp.replace("rsrp=", "");

            int rsrp = Integer.parseInt(css[2].replace("rsrp=", ""));
            if(rsrp>0)
            {
                rsrp *= -1;
                asuLevel = rsrp + 140;
            }
            if(dBm>0)
            {
                dBm *= -1;
            }




            cellInfoObject.put("asu", asuLevel);
            cellInfoObject.put("dBm", dBm);
            cellInfoObject.put("level", level);
            cellInfoObject.put("timingAdvance", timingAdvance);




            String rsrq = css[3];
            rsrq = rsrq.replace("rsrq=", "");

            String snr = css[4];
            snr = snr.replace("rssnr=", "");

            String cqi = css[5];
            cqi = cqi.replace("cqi=", "");

            String ta = css[6];
            ta = ta.replace("ta=", "");


            cellInfoObject.put("ss", Integer.parseInt(ss));
            cellInfoObject.put("rsrp", rsrp);
            cellInfoObject.put("rsrq", Integer.parseInt(rsrq));
            cellInfoObject.put("snr", Integer.parseInt(snr));
            cellInfoObject.put("cqi", Integer.parseInt(cqi));
            cellInfoObject.put("ta", Integer.parseInt(ta));


            cellInfoObject.put("ci", cellIdentity.getCi());
            //cellInfoObject.put("earfcn", earfcn);
            cellInfoObject.put("mcc", cellIdentity.getMcc());
            cellInfoObject.put("mnc", cellIdentity.getMnc());
            cellInfoObject.put("pci", cellIdentity.getPci());
            cellInfoObject.put("tac", cellIdentity.getTac());


            cellInfoObject.put("raw", cellSignalStrength.toString());
            cellInfoObject.put("type", "LTE");
        }
        catch(JSONException ex){
            System.out.println("JSON error");
        }

        return cellInfoObject;
    }



    public static String scanNetworks()
    {
        String result = "";

        ConnectivityManager cm = (ConnectivityManager) m_instance.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo[] infos = cm.getAllNetworkInfo();

        for( int i=0; i<infos.length; i++)
        {
            NetworkInfo info = infos[i];


            //System.out.println(Environment.getExternalStorageDirectory().getAbsolutePath());

            //System.out.println(info.toString());
            //System.out.println(Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DOCUMENTS));
        }


        return result;
    }





    public static String readCellularInfo()
    {


        JSONObject jsonObj = new JSONObject();
        JSONArray gsmCellInfoArray = new JSONArray();
        JSONArray cdmaCellInfoArray = new JSONArray();
        JSONArray lteCellInfoArray = new JSONArray();

        String status = "invalid";
        try {
            final TelephonyManager tm = (TelephonyManager) m_instance.getSystemService(Context.TELEPHONY_SERVICE);

            for (final CellInfo info : tm.getAllCellInfo()) {
                if (info instanceof CellInfoGsm) {
                    gsmCellInfoArray.put(m_instance.parseGsm(info));
                    status = "valid";
                } else if (info instanceof CellInfoCdma) {
                    cdmaCellInfoArray.put(m_instance.parseCdma(info));
                    status = "valid";
                } else if (info instanceof CellInfoLte) {
                    lteCellInfoArray.put(m_instance.parseLte(info));
                    status = "valid";
                } else {
                    status = "unknown cell info";
                }
            }
        } catch (Exception e) {
            status = "no cell info";
        }

        try{
            jsonObj.put("status", status);
            jsonObj.put("lteCellInfo", lteCellInfoArray);
        }
        catch(JSONException ex){
            System.out.println("JSON error");
        }

        return jsonObj.toString();
    }
}
