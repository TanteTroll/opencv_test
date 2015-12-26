#include "convert.h"

#include <opencv2/opencv.hpp>
#include <QImage>
#include <QDebug>

convert::convert()
{

}

QImage convert::mat2qimg( const cv::Mat &inMat )
{
   switch ( inMat.type() )
   {
      // 8-bit, 4 channel
      case CV_8UC4:
      {
         QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB32 );

         return image;
      }

      // 8-bit, 3 channel
      case CV_8UC3:
      {
         QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_RGB888 );

         return image.rgbSwapped();
      }

      // 8-bit, 1 channel
      case CV_8UC1:
      {
         static QVector<QRgb>  sColorTable;

         // only create our color table once
         if ( sColorTable.isEmpty() )
         {
            for ( int i = 0; i < 256; ++i )
               sColorTable.push_back( qRgb( i, i, i ) );
         }

         QImage image( inMat.data, inMat.cols, inMat.rows, inMat.step, QImage::Format_Indexed8 );

         image.setColorTable( sColorTable );

         return image;
      }

      default:
         qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
         break;
   }

   return QImage();

}
cv::Mat convert::qimg2mat( const QImage &inImage, bool inCloneImageData )
{
   switch ( inImage.format() )
   {
      // 8-bit, 4 channel
      case QImage::Format_RGB32:
      {
         cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC4, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );

         return (inCloneImageData ? mat.clone() : mat);
      }

      // 8-bit, 3 channel
      case QImage::Format_RGB888:
      {
         if ( !inCloneImageData )
            qWarning() << "ASM::QImageToCvMat() - Conversion requires cloning since we use a temporary QImage";

         QImage   swapped = inImage.rgbSwapped();

         return cv::Mat( swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine() ).clone();
      }

      // 8-bit, 1 channel
      case QImage::Format_Indexed8:
      {
         cv::Mat  mat( inImage.height(), inImage.width(), CV_8UC1, const_cast<uchar*>(inImage.bits()), inImage.bytesPerLine() );

         return (inCloneImageData ? mat.clone() : mat);
      }

      default:
         qWarning() << "ASM::QImageToCvMat() - QImage format not handled in switch:" << inImage.format();
         break;
   }

   return cv::Mat();
}
QImage convert::rotate_cw(QImage image, int degrees)
{
    image = image.transformed(QTransform().rotate(degrees));
    return image;
}
cv::Mat convert::rotate_cw( cv::Mat image, int degrees)
{
   cv::Mat res;
   switch (degrees % 360) {
       case 0:
           res = image;
           break;
       case 90:
           res = image.t();
           cv::flip(res, res, 1);
           break;
       case 180:
           cv::flip(image, res, -1);
           break;
       case 270:
           res = image.t();
           cv::flip(res, res, 0);
           break;
       default:;
           cv::Mat r = cv::getRotationMatrix2D({image.cols/2.0F, image.rows/2.0F}, degrees, 1.0);
           int len = std::max(image.cols, image.rows);
           cv::warpAffine(image, res, r, cv::Size(len, len));
           break; //image size will change
   }
   return res;
}
