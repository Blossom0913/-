typedef BOOL bool;
typedef TRUE true;
typedef FALSE false;
// ����һ��ģʽ�Ի���
void CTest::OnTest(){
	CMyDialog dlg;
	dlg.DoModal();
}

//  ����һ����ģʽ�Ի���

void CTestView::OnTest(){

	static BOOL bFlag = FALSE;
	if(FALSE == bFlag)
	{
		dlg.Create(IDD_DIALOG,this);
		dlg.ShowWindow(SW_SHOW);
		bFlag = TRUE;
	}
	else
	{
		dlg.DestroyWindow();
		bFlag = FALSE;
	}
}