//#define _DEBUG !NDEBUG // from CMake

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>

#include <iostream>

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusUnixFileDescriptor>
#include <QImage>


static const QString s_screenShotService = QStringLiteral("org.kde.KWin.ScreenShot2");
static const QString s_screenShotObjectPath = QStringLiteral("/org/kde/KWin/ScreenShot2");
static const QString s_screenShotInterface = QStringLiteral("org.kde.KWin.ScreenShot2");
static const QString s_screenShotMethod = QStringLiteral("CaptureActiveScreen");
static const int BUF_SIZE = 0x1000;

int main()
{
	struct timespec timestamp;
	clock_gettime(CLOCK_REALTIME, &timestamp);
	auto tm = localtime(&timestamp.tv_sec);

	int pipeFds[2];
	if (pipe2(pipeFds, O_CLOEXEC) == -1) {
		perror("Can not create pipe for screenshot.\n");
		return -1;
	}

	QVariantMap options{};
	options.insert(QStringLiteral("native-resolution"), true);
	//options.insert(QStringLiteral("include-decoration"), true);
	//options.insert(QStringLiteral("include-cursor"), true);
	QDBusInterface interface(s_screenShotService, s_screenShotObjectPath, s_screenShotInterface);

	QDBusReply<QVariantMap> rep
		= interface.call(
			s_screenShotMethod,
			options,
			QVariant::fromValue(QDBusUnixFileDescriptor(pipeFds[1])));

#ifdef _DEBUG
	std::cout << "rep.isValid() = " << rep.isValid() << "\n";
	std::cout << "rep.error().isValid() = " << rep.error().isValid() << "\n";
	std::cout << rep.error().message().toStdString() << "\n\n";
	for (auto kv = rep.value().constKeyValueBegin(); kv != rep.value().constKeyValueEnd(); kv++)
	{
		std::cout << kv->first.toStdString() << " = (" << kv->second.typeName() << ") " << kv->second.toString().toStdString() << "\n";
	}
#endif

	if (!rep.isValid())
	{
		// TODO: Logging
		std::cerr << rep.error().message().toStdString() << "\n";
		close(pipeFds[0]);
		close(pipeFds[1]);
		return -2;
	}


	QMap<QString, QVariant> repv{rep.value()};
	int width = repv["width"].toInt();
	int height = repv["height"].toInt();
	int stride = repv["stride"].toInt();
	int format = repv["format"].toInt();
	QString type = repv["type"].toString();

	if (type != "raw")
	{
		// TODO: Logging
		std::cerr << "Warning: screenshot format is not 'raw'. Proceeding, but data may be corrupt.\n";
	}

	QImage img(width, height, (QImage::Format)format);

	size_t needToRead = stride * height;
	size_t nReadTotal = 0;
	errno = 0;
	while (true)
	{
		size_t remaining = needToRead - nReadTotal;
		size_t readRequestLen = (BUF_SIZE < remaining) ? BUF_SIZE : remaining;

#ifdef _DEBUG
		std::cout << nReadTotal << " / " << needToRead << " bytes (" << remaining << " remaining) ";
#endif

		int nread = read(pipeFds[0], img.bits() + nReadTotal, readRequestLen);
		if (nread == -1)
		{
			perror("Pipe read error");
			break;
		}
		if (nread == 0) { break; }
		nReadTotal += nread;
		if (nReadTotal >= needToRead) { break; }
	}

	close(pipeFds[0]);
	close(pipeFds[1]);

#ifdef _DEBUG
	std::cout << "\nReceived " << nReadTotal << " bytes.\n";
#endif

	QString home = getenv("HOME");
	std::string dir = home.toStdString() + "/Screenshots/";
	mkdir(dir.c_str(), 0700);
	errno = 0;

	std::string filename = dir;

	char filenamebuf[100];
	sprintf(filenamebuf, "%.4d%.2d%.2d-%.2d%.2d%.2d-%lld.png",
			tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
			tm->tm_hour, tm->tm_min, tm->tm_sec,
			(long long)timestamp.tv_nsec);
	filename += filenamebuf;

	if (!img.save(filename.c_str(), "PNG", 100))
	{
		// TODO: Logging
		std::cerr << "Failed to save PNG\n";
		return -5;
	}

	std::cout << filename;

	// TODO: Logging, time of operation

	return 0;
}  //main()
