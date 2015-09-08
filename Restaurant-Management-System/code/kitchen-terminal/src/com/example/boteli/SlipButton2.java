package com.example.boteli;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.Rect;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;

public class SlipButton2 extends View implements OnTouchListener{
 
 public  boolean IsOpen;//��¼��ǰ��ť�Ƿ��,trueΪ��,flaseΪ�ر� ���˵��޸� 
    private boolean OnSlip = false;//��¼�û��Ƿ��ڻ����ı���  
    private float DownX,NowX;//����ʱ��x,��ǰ��x,  
    private Rect Btn_On,Btn_Off;//�򿪺͹ر�״̬��,�α��Rect  
    private boolean isChgLsnOn = false; 
    private OnChangedListener2 ChgLsn; 
    private Bitmap bg_on,bg_off,slip_btn; 
    public SlipButton2(Context context) { 
        super(context);
        IsOpen = true;
        // TODO Auto-generated constructor stub  
        init(); 
    } 
    public SlipButton2(Context context, AttributeSet attrs) { 
        super(context, attrs);
        IsOpen = true;
        // TODO Auto-generated constructor stub  
        init(); 
    } 
    private void init(){//��ʼ��  
        //����ͼƬ��Դ  
        bg_on = BitmapFactory.decodeResource(getResources(), R.drawable.on_btn2); 
        bg_off = BitmapFactory.decodeResource(getResources(), R.drawable.off_btn2); 
        slip_btn = BitmapFactory.decodeResource(getResources(), R.drawable.white_btn); 
        //�����Ҫ��Rect����  
        Btn_On = new Rect(0,0,slip_btn.getWidth(),slip_btn.getHeight()); 
        Btn_Off = new Rect( 
                bg_off.getWidth()-slip_btn.getWidth(), 
                0, 
                bg_off.getWidth(), 
                slip_btn.getHeight()); 
        setOnTouchListener(this);//���ü�����,Ҳ����ֱ�Ӹ�дOnTouchEvent  
    } 
     
    @Override 
    protected void onDraw(Canvas canvas) {//��ͼ����  
        // TODO Auto-generated method stub  
        super.onDraw(canvas); 
        Matrix matrix = new Matrix(); 
        Paint paint = new Paint(); 
        float x; 

     //���������жϳ�ʼ״̬
        if(IsOpen){
         NowX=bg_on.getWidth();
       }else{
         NowX=0;
       }
        { 
            if(NowX<(bg_on.getWidth()/2))//������ǰ�������εı�����ͬ,�ڴ����ж�  
                canvas.drawBitmap(bg_off,matrix, paint);//�����ر�ʱ�ı���  
            else 
                canvas.drawBitmap(bg_on,matrix, paint);//������ʱ�ı���  
             
            if(OnSlip)//�Ƿ����ڻ���״̬,  
            { 
                if(NowX >= bg_on.getWidth())//�Ƿ񻮳�ָ����Χ,�������α��ܵ���ͷ,����������ж�  
                    x = bg_on.getWidth()-slip_btn.getWidth()/2;//��ȥ�α�1/2�ĳ���...  
               
                else 
                    x = NowX - slip_btn.getWidth()/2;
            }else{//�ǻ���״̬  
                if(IsOpen)//�������ڵĿ���״̬���û��α��λ��  
                    x = Btn_Off.left; 
                else 
                    x = Btn_On.left; 
            } 
        if(x<0)//���α�λ�ý����쳣�ж�...  
            x = 0; 
        else if(x>bg_on.getWidth()-slip_btn.getWidth()) 
            x = bg_on.getWidth()-slip_btn.getWidth(); 
        canvas.drawBitmap(slip_btn,x, 0, paint);//�����α�.  
        } 
    }   
    @Override 
    public boolean onTouch(View v, MotionEvent event) { 
        // TODO Auto-generated method stub  
        switch(event.getAction())//���ݶ�����ִ�д���  
        { 
        case MotionEvent.ACTION_MOVE://����  
            NowX = event.getX(); 
            break; 
        case MotionEvent.ACTION_DOWN://����  
        if(event.getX()>bg_on.getWidth()||event.getY()>bg_on.getHeight()) 
            return false; 
            OnSlip = true; 
            DownX = event.getX(); 
            NowX = DownX; 
            break; 
        case MotionEvent.ACTION_UP://�ɿ�  
            OnSlip = false; 
            boolean LastChoose = IsOpen; 
            if(event.getX()>=(bg_on.getWidth()/2)) 
             IsOpen = true; 
            else 
             IsOpen = false; 
            if(isChgLsnOn&&(LastChoose!=IsOpen))//��������˼�����,�͵����䷽��..  
                ChgLsn.OnChanged(IsOpen); 
            break; 
        default: 
         
        } 
        invalidate();//�ػ��ؼ�  
        return true; 
    }     
    public void SetOnChangedListener(OnChangedListener2 l){//���ü�����,��״̬�޸ĵ�ʱ��  
        isChgLsnOn = true; 
        ChgLsn = l; 
    }    
public interface OnChangedListener2 {
 abstract void OnChanged(boolean CheckState); 

}
} 

//��������




