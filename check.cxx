#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"

#include "itkBinaryPerimeterEstimationCalculator.h"


int main(int argc, char * argv[])
{

  if( argc != 3 )
    {
    std::cerr << "usage: " << argv[0] << " img fg" << std::endl;
    // std::cerr << "  : " << std::endl;
    exit(1);
    }

#if defined(DIM)
  const int dim = DIM;
#else
  const int dim = 3;
#endif
  
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;

  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  reader->Update();

  typedef itk::BinaryPerimeterEstimationCalculator< IType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetImage( reader->GetOutput() );
  filter->SetForegroundValue( atoi(argv[2]) );

  // itk::SimpleFilterWatcher watcher(filter, "filter");

  filter->Compute();
  std::cout << filter->GetPerimeter() << std::endl;

  return 0;
}

