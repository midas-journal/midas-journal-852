#include "itkImageFileReader.h"
#include "itkShapeLabelObject.h"
#include "itkLabelMap.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "itkShapeLabelMapFilter.h"

int main(int argc, char * argv[])
{
#if defined(DIM)
  const int dim = DIM;
#else
  const int dim = 3;
#endif

  typedef unsigned char PixelType;
  typedef itk::Image< PixelType, dim >    ImageType;
  
  if( argc != 2)
    {
    std::cerr << "usage: " << argv[0] << " input" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef unsigned long LabelType;
  typedef itk::ShapeLabelObject< LabelType, dim > LabelObjectType;
  typedef itk::LabelMap< LabelObjectType > LabelMapType;

  // convert the image in a collection of objects
  typedef itk::LabelImageToLabelMapFilter< ImageType, LabelMapType > ConverterType;
  ConverterType::Pointer converter = ConverterType::New();
  converter->SetInput( reader->GetOutput() );

  typedef itk::ShapeLabelMapFilter< LabelMapType > ValuatorType;
  ValuatorType::Pointer valuator = ValuatorType::New();
  valuator->SetInput( converter->GetOutput() );

  valuator->Update();

  LabelMapType::Pointer labelMap = valuator->GetOutput();
  for( unsigned int label=0; label<labelMap->GetNumberOfLabelObjects(); label++ )
    {
    const LabelObjectType * labelObject = labelMap->GetNthLabelObject( label );
    std::cout << labelObject->GetLabel() << "\t" << labelObject->GetPhysicalSize() << "\t" << labelObject->GetPerimeter() << std::endl;
    }
  
  return 0;
}

