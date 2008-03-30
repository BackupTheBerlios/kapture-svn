#include <QProgressBar>
#include <QString>
#include <QHBoxLayout>
#include <QLabel>


class FileTransferWidget
{
public:
	FileTransferWidget();
	FileTransferWidget(QString&);
	~FileTransferWidget();
	void setPourcentage(int);
	QHBoxLayout *box();
	QString fileName() const;
	
	QProgressBar* fileTransferBar;
	QLabel *lab;
	QHBoxLayout *hboxlayout;
	QString f;

};
